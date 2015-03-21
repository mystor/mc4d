#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

// The location of the eye in space
uniform vec4 eye;

// The worldToEyeMat4D, used by the 4d => 3d projection
uniform mat4 worldToEyeMat4D;

// The reciprocal of the tangent of the view angle over 2 (used by 4d => 3d projection)
uniform float recipTanViewAngle;

// The 3d projection matrix
uniform mat4 projMat3D;

// The points in space (and the # of them)
uniform sampler1D hypercube;
uniform float hcCount;

// Passing the color to the geometry shader
out vec4 vcolor;

// This function accepts some pre-computed values which will help speed stuff up
vec4 projectTo3D()
{
  // Get the position of the hypercube based on a texture lookuo
  // TODO(michael): support more cells by using 2D textures instead of 1D ones
  vec4 realPosition = position + texture(hypercube, gl_InstanceID / hcCount);

  // Get the position in eye-space (offset)
  vec4 eyePos = realPosition - eye;

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

  // Outputs
  vcolor = color;
  gl_Position = pos2;
}
