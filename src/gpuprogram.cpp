// GPUProgram class


#include "gpuProgram.h"


void ShaderProgram::validateShader(GLuint shader) {

  const unsigned int BUFFER_SIZE = 512;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  GLsizei length = 0;

  glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

  if (length > 0)
    std::cout << "Shader " << shader << " compile log: " << std::endl << buffer << std::endl;
}

void ShaderProgram::validateProgram() {
  const unsigned int BUFFER_SIZE = 512;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  GLsizei length = 0;

  memset(buffer, 0, BUFFER_SIZE);

  glGetProgramInfoLog(progId, BUFFER_SIZE, &length, buffer);

  if (length > 0)
    std::cout << "Program " << progId << " link log: " << buffer << std::endl;

  glValidateProgram(progId);

  GLint status;
  glGetProgramiv(progId, GL_VALIDATE_STATUS, &status);

  if (status == GL_FALSE) {
    std::cerr << "Error validating program " << progId << std::endl;

    GLint logSize = 0;
    glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &logSize);

    char infoLog[logSize];
    memset(infoLog, 0, logSize);
    glGetProgramInfoLog(progId, logSize, NULL, infoLog);

    std::cerr << "Log: " << infoLog << std::endl;

    // Exit the program due to the error!
    // exit(-1);
  }
}
