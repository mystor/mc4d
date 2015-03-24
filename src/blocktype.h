//===--- blocktype.h - A block type for rendering purposes ------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __blocktype_h
#define __blocktype_h

#include "gl.h"
#include <assert.h>
#include <glm/glm.hpp>
#include <vector>

struct BlockType {
  GLuint tex;
  size_t count;
  float indicator;

  BlockType(std::vector<glm::vec4> *pts, float ind) {
    count = pts->size();
    indicator = ind;

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    std::cout << "Texture " << tex << " has " << count << " tesseracts\n";
    // Ensure that we have a small enough set
    assert((GLint) count < maxTextureSize);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA,
                 count,
                 0, GL_RGBA, GL_FLOAT,
                 pts->data());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_ERR_CHK;
  }

  void bind(GLuint texLoc, GLuint countLoc, GLuint indLoc) {
    // Bind the texture
    glUniform1i(texLoc, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_1D, tex);

    // Bind the count
    glUniform1f(countLoc, count);

    // Bind the indicator
    glUniform1f(indLoc, indicator);
    GL_ERR_CHK;
  }
};

#endif // defined(__blocktype_h)
