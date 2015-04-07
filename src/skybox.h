//===--- skybox.h - Generates & Renders the skybox --------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __skybox_h
#define __skybox_h

// Cimg image parsing library
#include "readpng.h"
#include "gl.h"
#include <glm/glm.hpp>


struct Skybox {
  static const int width  = 128;
  static const int height = 128;

  GLuint texture;
  GLuint VAO, VBO;

  Skybox() {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height;
    loadTexture("res/frontImage.png", width, height, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    loadTexture("res/backImage.png", width, height, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    loadTexture("res/upImage.png", width, height, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    loadTexture("res/downImage.png", width, height, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    loadTexture("res/rightImage.png", width, height, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    loadTexture("res/leftImage.png", width, height, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    /*
glm::vec4 data[width * height];
    memset(data, 0, sizeof(data));
    srand(1623);
    for (int i=0; i<width * height; i++) {
      if (rand() < 0.1 * RAND_MAX) {
        data[i] = glm::vec4(144/255.0, 96/255.0, 144/255.0, 1);
      } else {
        data[i] = glm::vec4(0, 24/255.0, 72/255.0, 1);
      }
    }

    // Create the texture
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_ERR_CHK;


    GLenum targets[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                         GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                         GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                         GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

    for (size_t i = 0; i < 6; i++) {
      glTexImage2D(targets[i], 0, GL_RGBA,
                   width, height,
                   0, GL_RGBA, GL_FLOAT,
                   data);
      GL_ERR_CHK;
    }
    */

    float verts[] = {
      -1.0f,-1.0f,-1.0f,
      -1.0f,-1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f,-1.0f,
      -1.0f,-1.0f,-1.0f,
      -1.0f, 1.0f,-1.0f,
      1.0f,-1.0f, 1.0f,
      -1.0f,-1.0f,-1.0f,
      1.0f,-1.0f,-1.0f,
      1.0f, 1.0f,-1.0f,
      1.0f,-1.0f,-1.0f,
      -1.0f,-1.0f,-1.0f,
      -1.0f,-1.0f,-1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f,-1.0f,
      1.0f,-1.0f, 1.0f,
      -1.0f,-1.0f, 1.0f,
      -1.0f,-1.0f,-1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f,-1.0f, 1.0f,
      1.0f,-1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f,-1.0f,-1.0f,
      1.0f, 1.0f,-1.0f,
      1.0f,-1.0f,-1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f,-1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f,-1.0f,
      -1.0f, 1.0f,-1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f,-1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f,-1.0f, 1.0f
    };
  }

  void draw() {
  }
};

#endif // defined(__skybox_h)
