//===--- project.h - Utilities for 4D and 3D projection ---------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __project_h
#define __project_h

#include "gl.h"
#include <glm/glm.hpp>

// Compute the 4D cross product of 3 4D vectors
glm::vec4 cross4(glm::vec4 a, glm::vec4 b, glm::vec4 c);

// Values required for the 4D->3D projection
glm::mat4 calcWorldToEyeMat4D(glm::vec4 up, glm::vec4 over, glm::vec4 forward);
float calcInvTanViewAngle(float viewAngle);

// Value required for the 3D->2D projection
glm::mat4 calcProjMat3D(float viewAngle, float aspectRatio, bool ortho);
// glm::mat4 calcProjMat3D(glm::vec3 eye, glm::vec3 up, glm::vec3 forward);

#endif // defined(__project_h)
