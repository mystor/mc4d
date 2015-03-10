//===--- viewport.h - Simple VBO filling entire viewport --------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __viewport_h
#define __viewport_h

#include "gl.h"

struct View {
  GLuint VAO;
  uint32_t size;

  View() {
    float verts[] = {
      -1, 1,
      1, 1,
      1, -1,
      -1, -1
    };

    GLuint VBO;

    // Create & Bind the VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create & Bind the VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Load the data
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(verts),
                 verts,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    size = sizeof(verts)/sizeof(verts[0]);
    // View view = { VAO, sizeof(verts)/sizeof(verts[0]) };
    // return view;
  }

  // Draws the view represented by the VAO
  void draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, size);
  }
};

#endif // defined(__viewport_h)
