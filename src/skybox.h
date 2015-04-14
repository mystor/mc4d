//===--- skybox.h - Generates & Renders the skybox --------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __skybox_h
#define __skybox_h

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl.h"

#include "readpng.h"
#include "gpuprogram.h"
#include "shaders.h"

static const float skybox_verts[] = {
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


struct Skybox {
  static const int width  = 128;
  static const int height = 128;

  GLuint texture;
  GLuint VAO, VBO;

  ShaderProgram sp;
  GLuint sbLoc, MVPLoc, eye3Loc;

  Skybox() {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height;
    loadTexture("res/leftImage.png", width, height, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    loadTexture("res/rightImage.png", width, height, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    loadTexture("res/downImage.png", width, height, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    loadTexture("res/upImage.png", width, height, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    loadTexture("res/frontImage.png", width, height, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    loadTexture("res/backImage.png", width, height, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(skybox_verts),
                 skybox_verts,
                 GL_STATIC_DRAW);
    GL_ERR_CHK;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    GL_ERR_CHK;

    sp.createShader(GL_VERTEX_SHADER, sbvertGlsl);
    sp.createShader(GL_FRAGMENT_SHADER, sbfragGlsl);
    sp.link();
    GL_ERR_CHK;

    sbLoc = sp.uniformLocation("sb");
    MVPLoc = sp.uniformLocation("MVP");
    eye3Loc = sp.uniformLocation("eye3");
  }

  void draw(glm::mat4 MVP, glm::vec3 eye3) {
    glBindVertexArray(VAO);

    sp.activate();
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP)); GL_ERR_CHK;
    glUniform3fv(eye3Loc, 1, glm::value_ptr(eye3)); GL_ERR_CHK;
    glUniform1i(sbLoc, 3); GL_ERR_CHK;

    glDrawArrays(GL_TRIANGLES, 0, sizeof(skybox_verts)/sizeof(skybox_verts[0]));
  }
};

#endif // defined(__skybox_h)
