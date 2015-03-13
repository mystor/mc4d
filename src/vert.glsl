#version 410

layout (location = 0) in vec4 position;

// The location of the eye in space
uniform vec4 eye;

// The worldToEyeMat4D, used by the 4d => 3d projection
uniform mat4 worldToEyeMat4D;

// The reciprocal of the tangent of the view angle over 2 (used by 4d => 3d projection)
uniform float recipTanViewAngle;

// The 3d projection matrix
uniform mat4 projMat3D;

// Scene rotation matrix
uniform mat4 srm;

// This function accepts some pre-computed values which will help speed stuff up
vec4 projectTo3D()
{
  vec4 eyePos = (srm * position) - eye;

  float scale = recipTanViewAngle / dot(eyePos, worldToEyeMat4D[3]);

  // TODO(michael): There is probably a better way to do this using
  // swizzling and other builtins
  return vec4(scale * dot(eyePos, worldToEyeMat4D[0]),
              scale * dot(eyePos, worldToEyeMat4D[1]),
              scale * dot(eyePos, worldToEyeMat4D[2]),
              1);
}

void main() {
  // Project into 3-space
  vec4 pos3 = projectTo3D();
  vec4 pos2 = projMat3D * pos3;

  // screenPos = position; // Ranges from -1 to 1 in each dimension
  gl_Position = pos2;
}
