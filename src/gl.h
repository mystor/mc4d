//===--- gl.h - OpenGl Includes ---------------------------------*- C++ -*-===//
//
//                              MC 4D Renderer
//                        Michael Layzell - CISC 454
//                        Queen's University - W2015
//
//===----------------------------------------------------------------------===//

#ifndef __gl_h
#define __gl_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define GL_ERR_HACK_1(x) #x
#define GL_ERR_HACK_2(x) GL_ERR_HACK_1(x)
#define GL_ERR_HACK __FILE__ ":" GL_ERR_HACK_2(__LINE__)

#define GL_ERR_CHK glErrChk(GL_ERR_HACK)

static inline void glErrChk(const char *where = "") {
  const char *errStr;
  GLenum errCode = glGetError();

  switch (errCode) {
  case GL_NO_ERROR:
    return;
  case GL_INVALID_ENUM:
    errStr = "Invalid Enum"; break;
  case GL_INVALID_VALUE:
    errStr = "Invalid Value"; break;
  case GL_INVALID_OPERATION:
    errStr = "Invalid Operation"; break;
  case GL_STACK_OVERFLOW:
    errStr = "Stack Overflow"; break;
  case GL_STACK_UNDERFLOW:
    errStr = "Stack Underflow"; break;
  case GL_OUT_OF_MEMORY:
    errStr = "Out of Memory"; break;
  case GL_TABLE_TOO_LARGE:
    errStr = "Table too large"; break;
  default:
    errStr = "Unknown Error"; break;
  }

  std::cerr << "OpenGL Error(" << where << "): " << errStr << std::endl;

  // if ((errCode = glGetError()) != GL_NO_ERROR) {
  //   errString = gluErrorString(errCode);
  //   std::cerr << "OpenGL Error(" << where << "): " << errString << std::endl;
  // }
}

#endif // defined(__gl_h)
