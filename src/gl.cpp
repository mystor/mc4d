#include "gl.h"

// Report any errors. Handled mostly by the GL_ERR_CHK macro
void glErrChk(const char *where) {
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
}

// This is a really simple FPS thing from http://r3dux.org/2012/07/a-simple-glfw-fps-counter/
// I am just using it mostly unchanged to get an idea of what the FPS is like in the program
double calcFPS(double theTimeInterval) {
  // Static values which only get initialised the first time the function runs
  static double t0Value       = glfwGetTime(); // Set the initial time to now
  static int    fpsFrameCount = 0;             // Set the initial FPS frame count to 0
  static double fps           = 0.0;           // Set the initial FPS value to 0.0

  // Get the current time in seconds since the program started (non-static, so executed every time)
  double currentTime = glfwGetTime();

  // Ensure the time interval between FPS checks is sane (low cap = 0.1s, high-cap = 10.0s)
  // Negative numbers are invalid, 10 fps checks per second at most, 1 every 10 secs at least.
  if (theTimeInterval < 0.1)
    {
      theTimeInterval = 0.1;
    }
  if (theTimeInterval > 10.0)
    {
      theTimeInterval = 10.0;
    }

  // Calculate and display the FPS every specified time interval
  if ((currentTime - t0Value) > theTimeInterval)
    {
      // Calculate the FPS as the number of frames divided by the interval in seconds
      fps = (double)fpsFrameCount / (currentTime - t0Value);

      std::cout << "FPS: " << fps << std::endl;

      // Reset the FPS frame counter and set the initial time to be now
      fpsFrameCount = 0;
      t0Value = glfwGetTime();
    }
  else // FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
    {
      fpsFrameCount++;
    }

  // Return the current FPS - doesn't have to be used if you don't want it!
  return fps;
}
