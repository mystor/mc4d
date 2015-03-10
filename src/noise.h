//===--- noise.h - 3D/4D noise generation functions -------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __noise_h
#define __noise_h

#include <glm/glm.hpp>

// This has to be called first!
void initPerm();

double simplexNoise4D(double x, double y, double z, double w);
double simplexNoise4D(glm::dvec4 pos);

#endif // defined(__noise_h)
