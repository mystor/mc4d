//===--- world.h - The Game World -------------------------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __world_h
#define __world_h

#include "gl.h"

#include <vector>
#include <glm/glm.hpp>

class World {
  // GLuint VAO;
  // GLuint VBO;

  std::vector<glm::vec4> verts;
public:
  World();
};

#endif // defined(__world_h)
