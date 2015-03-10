#include "controls.h"
// #include "tesseract.h"
#include "config.h"
#include "gpuProgram.h"
// #include "noise.h"
#include "world.h"

#include "fragShader.h"
#include "vertShader.h"

#include "gl.h"

#include <stdlib.h>
#include <iostream>

double calcFPS(double theTimeInterval = 1.0);

/// This function is called when the
static void errorCallback(int error, const char* description)
{
  std::cerr << "GLFW ERROR: " << error << ": " << description << std::endl;
  fputs(description, stderr);
}


int main(int argc, char **argv)
{
  std::cout << vertGlsl << std::endl;
  std::cout << fragGlsl << std::endl;
  Config::init(argc, argv);

  GLFWwindow* window;
  glfwSetErrorCallback(errorCallback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  // Ensure that we have the correct version of OpenGl context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create the window
  window = glfwCreateWindow(640, 480, "mc4d", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, keyCallback);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glErrChk("BEFORE_GLEW_ERROR");
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    std::cerr << "Error: " << glewGetErrorString(glewError) << std::endl;
    exit(EXIT_FAILURE);
  }

  glErrChk("GLEW_ERROR");

  // glEnable(GL_TEXTURE_3D); glErrChk("Texture_3D check");

  // View view = View();

  World world = World();

#if 1
  // Early exit to avoid actually doing anything
  return 0;
#endif

  glErrChk("Before GP");
  // Create an initialize the GPU program which does basically all
  // of the actual rendering for the program
  GPUProgram gp;
  gp.init(vertGlsl, fragGlsl);
  gp.activate();
  glErrChk("After GPUProgram");

  /* glActiveTexture(GL_TEXTURE0);
  NoiseTexture worldTex(0); // TODO(michael): Find a good seed
  glErrChk("BeforeBInd");

  worldTex.bindToUniform(0, gp.program_id, "world");
  glErrChk("After WorldTex"); */







  // Set the eye, forward, up, and right vectors
  /* GLuint eyeLoc = glGetUniformLocation(gp.program_id, "eye");
  GLuint forwardLoc = glGetUniformLocation(gp.program_id, "forward");
  GLuint upLoc = glGetUniformLocation(gp.program_id, "up");
  GLuint rightLoc = glGetUniformLocation(gp.program_id, "right");

  // These are the defaults for right now
  glUniform3f(eyeLoc, 64, 32, 0);
  glUniform3f(forwardLoc, 1, 0, 0);
  glUniform3f(upLoc, 0, 1, 0);
  glUniform3f(rightLoc, 0, 0, 1); */

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // TODO(michael): Is this necessary?
    glClearColor( 0.0, 0.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );

    // Draw the view
    // view.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();

    glErrChk("Main Loop");
    calcFPS();
  }

  // Exit the program
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}




// ----

// This is a really simple FPS thing from http://r3dux.org/2012/07/a-simple-glfw-fps-counter/
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
