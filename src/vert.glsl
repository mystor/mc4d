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

// Where are we looking (should be perpendicular)
uniform vec4 forward;
uniform vec4 up;
uniform vec4 over;

uniform float viewAngle;

// This is the location
// out vec2 screenPos;

// This implementation of cross4 is taken directly from Steve Hollasch's thesis:
// =*= http://steve.hollasch.net/thesis/chapter2.html =*=
// There may be a faster way to do it on the GPU, but it works
//
// TODO(michael): Is this necessary to have on the GPU?
// vec4 cross4(vec4 a, vec4 b, vec4 c) {
//   float A, B, C, D, E, F;       // Intermediate Values
//
//   // Calculate intermediate values.
//   A = (V.x * W.y) - (V.y * W.x);
//   B = (V.x * W.z) - (V.z * W.x);
//   C = (V.x * W.w) - (V.w * W.x);
//   D = (V.y * W.z) - (V.z * W.y);
//   E = (V.y * W.w) - (V.w * W.y);
//   F = (V.z * W.w) - (V.w * W.z);
//
//   // Calculate the result-vector components.
//   return vec4(+ (U.y * F) - (U.z * E) + (U.w * D),
//               - (U.x * F) + (U.z * C) - (U.w * B),
//               + (U.x * E) - (U.y * C) + (U.w * A),
//               - (U.x * D) + (U.y * B) - (U.z * A));
// }

/* mat4 calcWorldToEyeMatrix() {
  vec4 Wa, Wb, Wc, Wd;

  Wd = normalize(dir);

  // Calculate the normalized Wa column-vector.
  Wa = normalize(cross4(Up, Over, Wd));

  // Calculate the normalized Wb column-vector.
  Wb = normalize(cross4(Over, Wd, Wa));

  // Calculate the Wc column-vector.
  Wc = normalize(cross4(Wd, Wa, Wb));

  return mat4(Wa, Wb, Wc, Wd);
}

float calcInvTanViewAngle() {
} */

// This function accepts some pre-computed values which will help speed stuff up
vec4 projectTo3D()
{
  vec4 eyePos = position - eye;

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
