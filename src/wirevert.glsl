#version 410

layout (location = 0) in vec4 position;

// The location of the eye in space
uniform vec4 eye;
uniform vec4 forward;

// The worldToEyeMat4D, used by the 4d => 3d projection
uniform mat4 worldToEyeMat4D;

// The reciprocal of the tangent of the view angle over 2 (used by 4d => 3d projection)
uniform float recipTanViewAngle;

// The 3d projection matrix
uniform mat4 projMat3D;

// The srm
uniform mat4 srm;

out float depth;

// This function accepts some pre-computed values which will help speed stuff up
vec4 projectTo3D()
{
  vec4 realPosition = position;

  realPosition *= 16;
  realPosition *= srm;

  // Get the position in eye-space
  vec4 eyePos = (realPosition - eye) * worldToEyeMat4D;

  // Scale for perspective
  float scale = recipTanViewAngle / eyePos.w;
  depth = eyePos.w;
  return vec4(scale * eyePos.xyz, 1);
}

void main() {
  // Project into 3-space
  vec4 pos3 = projectTo3D();
  vec4 pos2 = projMat3D * pos3;

  // Outputs
  gl_Position = pos2;
}
