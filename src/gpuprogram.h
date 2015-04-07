// GPUProgram class

#ifndef SHADER_H
#define SHADER_H

#include "gl.h"
#include <iostream>

class ShaderProgram {
  void validateShader(GLuint shader);

public:
  void validateProgram();

  GLuint progId;

  ShaderProgram() {
    progId = glCreateProgram();
    GL_ERR_CHK;
  }

  void createShader(GLenum type, const char *body) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &body, 0);
    glCompileShader(shader);
    validateShader(shader);

    glAttachShader(progId, shader);
    GL_ERR_CHK;
  }

  void link() {
    glLinkProgram(progId);
    validateProgram();
    GL_ERR_CHK;
  }

  void activate() {
    glUseProgram(progId);
    GL_ERR_CHK;
  }

  GLuint uniformLocation(const char *name) {
    return glGetUniformLocation(progId, name);
  }
};

#endif
