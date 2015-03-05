//===--- noise.h - 3D/4D noise generation functions -------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __noise_h
#define __noise_h

#include "gl.h"

#define NOISE_WIDTH 16
#define NOISE_HEIGHT 16
#define NOISE_DEPTH 16

struct NoiseTexture {
  float data[NOISE_WIDTH * NOISE_HEIGHT * NOISE_DEPTH];
  GLuint texId;

  NoiseTexture(uint32_t seed);

  void bindToUniform(GLuint sampler, GLuint programId, const char *uniformName);
};

#endif // defined(__noise_h)
