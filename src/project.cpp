#include "project.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace glm;


// Definition of a 4d cross product from Steve Hollasch's thesis
// http://steve.hollasch.net/thesis/chapter2.html
vec4 cross4(vec4 a, vec4 b, vec4 c) {
  float A, B, C, D, E, F;       // Intermediate aalues

  // Calculate intermediate values.
  A = (a.x * b.y) - (a.y * b.x);
  B = (a.x * b.z) - (a.z * b.x);
  C = (a.x * b.w) - (a.w * b.x);
  D = (a.y * b.z) - (a.z * b.y);
  E = (a.y * b.w) - (a.w * b.y);
  F = (a.z * b.w) - (a.w * b.z);

  // Calculate the result-vector components.
  return vec4(+ (c.y * F) - (c.z * E) + (c.w * D),
              - (c.x * F) + (c.z * C) - (c.w * B),
              + (c.x * E) - (c.y * C) + (c.w * A),
              - (c.x * D) + (c.y * B) - (c.z * A));
}

// All 4d-3d projection code is adapted from steve hollasch's thesis
// http://steve.hollasch.net/thesis/chapter4.html
mat4 calcWorldToEyeMat4D(vec4 up, vec4 over, vec4 forward) {
  vec4 Wa, Wb, Wc, Wd;

  Wd = normalize(forward);

  // Calculate the normalized Wa column-vector.
  Wa = normalize(cross4(up, over, Wd));

  // Calculate the normalized Wb column-vector.
  Wb = normalize(cross4(over, Wd, Wa));

  // Calculate the Wc column-vector.
  Wc = normalize(cross4(Wd, Wa, Wb));

  return mat4(Wa, Wb, Wc, Wd);
}

float calcInvTanViewAngle(float viewAngle) {
  return 1.0f / tan(viewAngle * M_PI / 360.0f);
}


mat4 calcProjMat3D(float viewAngle, float aspectRatio, vec3 eyePos3) {
  mat4 projection, view;

  projection = perspective(viewAngle,
                           aspectRatio,
                           0.1f,
                           100.0f);
  view = lookAt(eyePos3, vec3(0, 0, 0), vec3(0, -1, 0));

  return projection * view;
}
