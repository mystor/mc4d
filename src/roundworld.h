//===--- roundworld.h - A round world ---------------------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __roundworld_h
#define __roundworld_h

#include "tesseract.h"

#include "gl.h"

#include <vector>
#include <glm/glm.hpp>

static const int32_t RDIM = 32;
static const int32_t RWD_X = RDIM;
static const int32_t RWD_Y = RDIM;
static const int32_t RWD_Z = RDIM;
static const int32_t RWD_W = RDIM;
static const glm::ivec4 ROUNDWORLD_DIM(RWD_X, RWD_Y, RWD_Z, RWD_W);

class RoundWorld {
  GLuint VAO;
  GLuint VBO;

  HyperCubeTypes hypercubes[RWD_X][RWD_Y][RWD_Z][RWD_W];

  HyperCubeTypes worldSample(int32_t x, int32_t y, int32_t z, int32_t w);
public:
  std::vector<glm::vec4> stoneLocs;
  std::vector<glm::vec4> grassLocs;
  std::vector<glm::vec4> sandLocs;
  std::vector<glm::vec4> waterLocs;

  RoundWorld();

  void draw();
};

#endif // defined(__roundworld_h)
