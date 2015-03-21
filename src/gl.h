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

// A hack to get a string containing the file and line number
#define GL_ERR_HACK_1(x) #x
#define GL_ERR_HACK_2(x) GL_ERR_HACK_1(x)
#define GL_ERR_HACK __FILE__ ":" GL_ERR_HACK_2(__LINE__)

#define GL_ERR_CHK glErrChk(GL_ERR_HACK)

void glErrChk(const char *where = "");

double calcFPS(double theTimeInterval = 1.0);

#endif // defined(__gl_h)
