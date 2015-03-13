#include "controls.h"
#include "tesseract.h"
#include "config.h"
#include "gpuProgram.h"
#include "world.h"
#include "project.h"

#include "fragShader.h"
#include "vertShader.h"

#include "gl.h"

#include <glm/gtc/type_ptr.hpp>

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
  glErrChk("GLEW_ERROR (OK)");

  // View view = View();

  // Before we can create the world, we need to initialize the tesseract
  Tesseract::gen();
  GL_ERR_CHK;

  // The world is heap allocated because otherwise it will blow out the stack
  // std::unique_ptr<World> world(new World());

  // --- TEST --- TESSERACT x1 ---
#define TESSERACT_LINES
#ifdef TESSERACT_LINES
  glm::vec4 verts[Tesseract::LINES_SIZE * 3];
  Tesseract::linesWithOffset(glm::vec4(0,0,0,0), verts);
  Tesseract::linesWithOffset(glm::vec4(0,0,1,0), verts + Tesseract::LINES_SIZE);
  Tesseract::linesWithOffset(glm::vec4(0,0,-1,0), verts + Tesseract::LINES_SIZE * 2);

  GLuint VAO, VBO;
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
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  GL_ERR_CHK;
#else // TRIS
  glm::vec4 verts[Tesseract::OUT_SIZE * 3];
  Tesseract::withOffset(glm::vec4(0,0,0,0), verts);
  Tesseract::withOffset(glm::vec4(0,0,1,0), verts + Tesseract::OUT_SIZE);
  Tesseract::withOffset(glm::vec4(0,0,-1,0), verts + Tesseract::OUT_SIZE * 2);

  GLuint VAO, VBO;
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
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  GL_ERR_CHK;
#endif

  // -- END TEST --

  // Create an initialize the GPU program which does basically all
  // of the actual rendering for the program
  GPUProgram gp;
  gp.init(vertGlsl, fragGlsl);
  gp.activate();
  GL_ERR_CHK;

#if 0 // Print max texture buffer sizes
  GLint maxTextureBufferSize;
  glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTextureBufferSize);
  std::cout << maxTextureBufferSize << std::endl;
#endif

  // The current state of the program
#if 0
  glm::vec4 up(0, 1, 0, 0);
  glm::vec4 over(0, 0, 1, 0);
  glm::vec4 forward(-1, 0, 0, 0);
  float viewAngle = 45;
  glm::vec4 eye(4, 0, 0, 0);
#endif

  glm::vec4 up(-.71, .71, 0, 0);
  glm::vec4 over(0, 0, 1, 0.02);
  glm::vec4 forward = normalize(glm::vec4(-2.83, -2.83, -0.01, 0));
  float viewAngle = 45;
  glm::vec4 eye(2.83, 2.83, 0.01, 0);

  glm::mat4 srm;

  // Get the location of the uniforms on the GPU
  GLuint worldToEyeMat4DLoc = glGetUniformLocation(gp.program_id, "worldToEyeMat4D");
  GLuint recipTanViewAngleLoc = glGetUniformLocation(gp.program_id, "recipTanViewAngle");
  GLuint projMat3DLoc = glGetUniformLocation(gp.program_id, "projMat3D");
  GLuint eyeLoc = glGetUniformLocation(gp.program_id, "eye");

  GLuint srmLoc = glGetUniformLocation(gp.program_id, "srm");

  // Main loop
  double lastTime, thisTime, delta;
  lastTime = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    // Calculate the time delta
    thisTime = glfwGetTime();
    delta = thisTime - lastTime;
    lastTime = thisTime;

    /* Get the screen aspect ratio */
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    // float R = M_PI * delta;
    // glm::mat4 rotMat(cosf(R), -sinf(R), 0, 0,
    // sinf(R), cosf(R), 0, 0,
    // 0, 0, 1, 0,
    // 0, 0, 0, 1);

    // eye *= R; // Rotate the eye
    // forward = normalize(-eye); // Look toward (0, 0, 0, 0)

    // Clear the color for the background
    glClearColor( 1.0, 0.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );

    // Calculate projecton stuff
    glm::mat4 worldToEyeMat4D = calcWorldToEyeMat4D(up, over, forward);
    float invTanViewAngle = calcInvTanViewAngle(viewAngle);

    // Value required for the 3D->2D projection
    glm::mat4 projMat3D = calcProjMat3D(viewAngle, ratio);

    // SRM
    float R = thisTime;
#if 0
    glm::mat4 srm = glm::mat4(cosf(R), -sinf(R), 0, 0,
                              sinf(R), cosf(R), 0, 0,
                              0, 0, 1, 0,
                              0, 0, 0, 1);
#endif
    glm::mat4 srm = glm::mat4(cosf(R), 0, 0, -sinf(R),
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              sinf(R), 0, 0, cosf(R));

    // Sending data to the GPU
    glUniform4fv(eyeLoc, 1, glm::value_ptr(eye)); GL_ERR_CHK;
    glUniform1f(recipTanViewAngleLoc, invTanViewAngle); GL_ERR_CHK;
    glUniformMatrix4fv(worldToEyeMat4DLoc, 1, GL_FALSE, glm::value_ptr(worldToEyeMat4D)); GL_ERR_CHK;
    glUniformMatrix4fv(projMat3DLoc, 1, GL_FALSE, glm::value_ptr(projMat3D)); GL_ERR_CHK;

    // Scene Rotation Matrix (for basic visualization purposes)
    glUniformMatrix4fv(srmLoc, 1, GL_FALSE, glm::value_ptr(srm)); GL_ERR_CHK;

    // Draw the tesseract
    glBindVertexArray(VAO);

    // Draw the tesseracts!
#ifdef TESSERACT_LINES
    glDrawArrays(GL_LINES, 0, sizeof(verts)/sizeof(verts[0]));
#else
    glDrawArrays(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]));
#endif
    GL_ERR_CHK;

    // world->draw();

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
