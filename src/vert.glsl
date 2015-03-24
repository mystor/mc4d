#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

// The location of the eye in space
uniform vec4 eye;
uniform vec4 forward;

// The worldToEyeMat4D, used by the 4d => 3d projection
uniform mat4 worldToEyeMat4D;

// The reciprocal of the tangent of the view angle over 2 (used by 4d => 3d projection)
uniform float recipTanViewAngle;

// The 3d projection matrix
uniform mat4 projMat3D;

// The points in space (and the # of them)
uniform sampler1D hypercube;
uniform float hcCount;

// The srm
uniform mat4 srm;

// Passing the color to the geometry shader
out vec4 vcolor;

// This function accepts some pre-computed values which will help speed stuff up
vec4 projectTo3D()
{
  // Get the position of the hypercube based on a texture lookuo
  // TODO(michael): support more cells by using 2D textures instead of 1D ones
  vec4 realPosition = position + texture(hypercube, gl_InstanceID / hcCount);

  // HACK(michael): Offset such that the world is centered at (0,0,0,0)
  realPosition -= vec4(8, 8, 8, 8);
  realPosition *= srm;

  // Get the position in eye-space
  vec4 eyePos = (realPosition - eye) * worldToEyeMat4D;

  // Scale for perspective
  float scale = recipTanViewAngle / eyePos.w;
  return vec4(scale * eyePos.xyz, 1);
}

void main() {
  vcolor = color;

  // Project into 3-space
  vec4 pos3 = projectTo3D();
  vec4 pos2 = projMat3D * pos3;

  // Outputs
  gl_Position = pos2;
}
