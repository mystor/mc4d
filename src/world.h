//===--- world.h - The Game World -------------------------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __world_h
#define __world_h

#include "tesseract.h"

#include "gl.h"

#include <vector>
#include <glm/glm.hpp>

static const int32_t DIM = 16;
static const int32_t WD_X = DIM;
static const int32_t WD_Y = DIM;
static const int32_t WD_Z = DIM;
static const int32_t WD_W = DIM;
static const glm::ivec4 WORLD_DIM(WD_X, WD_Y, WD_Z, WD_W);

class World {
  GLuint VAO;
  GLuint VBO;

  HyperCubeTypes hypercubes[WD_X][WD_Y][WD_Z][WD_W];

  HyperCubeTypes worldSample(int32_t x, int32_t y, int32_t z, int32_t w);
public:
  std::vector<glm::vec4> stoneLocs;
  std::vector<glm::vec4> grassLocs;
  std::vector<glm::vec4> sandLocs;
  std::vector<glm::vec4> waterLocs;

  World();

  void draw();
};

#endif // defined(__world_h)
