#include "controls.h"
#include "tesseract.h"
#include "config.h"
#include "gpuProgram.h"
#include "world.h"
#include "project.h"

#include "shaders.h"

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

  glEnable(GL_DEPTH_TEST);
  // TODO(michael): Add command line config option for this
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // View view = View();

  // Before we can create the world, we need to initialize the tesseract
  Tesseract::gen();
  GL_ERR_CHK;

  // The world is heap allocated because otherwise it will blow out the stack
  std::unique_ptr<World> world(new World());

  std::cout << world->hypercubeLocs.size() << std::endl;

  GLint maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

  std::cout << maxTextureSize << std::endl;
  assert((uint32_t)maxTextureSize >= world->hypercubeLocs.size());

  // Create the texture!
  GLuint hypercubeLocsTex;
  glGenTextures(1, &hypercubeLocsTex);
  glBindTexture(GL_TEXTURE_1D, hypercubeLocsTex);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA,
               world->hypercubeLocs.size(),
               0, GL_RGBA, GL_FLOAT,
               world->hypercubeLocs.data());
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  GL_ERR_CHK;


  // Surface Textures
  float faceTexPts[16*16];
  srand(223); // Arbitrarially chosen for the texture on the stone
  for (size_t i = 0; i < sizeof(faceTexPts)/sizeof(faceTexPts[0]); i++) {
    faceTexPts[i] = ((float) rand()) / RAND_MAX;
  }

  GLuint faceTex;
  glGenTextures(1, &faceTex);
  glBindTexture(GL_TEXTURE_2D, faceTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
               16, 16,
               0, GL_RED, GL_FLOAT,
               faceTexPts);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GL_ERR_CHK;


  // --- TEST --- TESSERACT x1 ---
  TesseractVert verts[Tesseract::OUT_SIZE];
  Tesseract::withOffset(glm::vec4(0,0,0,0), verts);
  GL_ERR_CHK;

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
  GL_ERR_CHK;

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 32, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 32, (void*) 16);
  GL_ERR_CHK;

  // -- END TEST --

  // Create an initialize the GPU program which does basically all
  // of the actual rendering for the program
  ShaderProgram mainShader;
  mainShader.createShader(GL_VERTEX_SHADER, vertGlsl);
  mainShader.createShader(GL_FRAGMENT_SHADER, fragGlsl);
  mainShader.createShader(GL_GEOMETRY_SHADER, geomGlsl);
  mainShader.link();
  mainShader.activate();
  GL_ERR_CHK;

  // glm::vec4 up(-.71, .71, 0, 0);
  glm::vec4 up(0, 1, 0, 0);
  glm::vec4 over(0, 0, 1, 0);
  // glm::vec4 forward = normalize(glm::vec4(-2.83, -2.83, -0.01, 0));
  // glm::vec4 forward = glm::vec4(1, 0, 0, 0);
  float viewAngle = 45;
  // glm::vec4 eye(2.83, 2.83, 0.01, 0);
  // glm::vec4 eye(-4, 0, 0, 0);
  glm::vec4 eye(-32, 32, 32, 32);

  glm::vec4 forward = normalize(-eye);

  glm::mat4 srm;

  // Get the location of the uniforms on the GPU
  GLuint worldToEyeMat4DLoc = mainShader.uniformLocation("worldToEyeMat4D");
  GLuint recipTanViewAngleLoc = mainShader.uniformLocation("recipTanViewAngle");
  GLuint projMat3DLoc = mainShader.uniformLocation("projMat3D");
  GLuint eyeLoc = mainShader.uniformLocation("eye");
  GLuint hypercubeLoc = mainShader.uniformLocation("hypercube");
  GLuint hcCountLoc = mainShader.uniformLocation("hcCount");
  GLuint faceTexLoc = mainShader.uniformLocation("faceTex");

  // Set the texture up
  glUniform1i(hypercubeLoc, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_1D, hypercubeLocsTex);

  // Send the count up to the GPU
  glUniform1f(hcCountLoc, world->hypercubeLocs.size());

  // Set the texture up
  glUniform1i(faceTexLoc, 2);
  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, faceTex);

  GLuint srmLoc = mainShader.uniformLocation("srm");

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
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


#if 0
    eye.y = fmod(thisTime/2, 15);
    eye.z = fmod(thisTime, 30);
    forward = normalize(-eye);
#endif

    // Calculate projecton stuff
    glm::mat4 worldToEyeMat4D = calcWorldToEyeMat4D(up, over, forward);
    float invTanViewAngle = calcInvTanViewAngle(viewAngle);

    // Value required for the 3D->2D projection
    glm::mat4 projMat3D = calcProjMat3D(viewAngle, ratio);

    // SRM
    float R = thisTime;
    // float R = 0;
    glm::mat4 srm = glm::mat4(cosf(R), 0, 0, -sinf(R),
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              sinf(R), 0, 0, cosf(R)) /* * glm::mat4 (cosf(R), -sinf(R), 0, 0,
                                                                  sinf(R), cosf(R), 0, 0,
                                                                  0, 0, 1, 0,
                                                                  0, 0, 0, 1) */;


    // Sending data to the GPU
    glUniform4fv(eyeLoc, 1, glm::value_ptr(eye)); GL_ERR_CHK;
    glUniform1f(recipTanViewAngleLoc, invTanViewAngle); GL_ERR_CHK;
    glUniformMatrix4fv(worldToEyeMat4DLoc, 1, GL_FALSE, glm::value_ptr(worldToEyeMat4D)); GL_ERR_CHK;
    glUniformMatrix4fv(projMat3DLoc, 1, GL_FALSE, glm::value_ptr(projMat3D)); GL_ERR_CHK;

    // Scene Rotation Matrix (for basic visualization purposes)
    glUniformMatrix4fv(srmLoc, 1, GL_FALSE, glm::value_ptr(srm)); GL_ERR_CHK;

    // Draw the tesseract

    // Draw the tesseracts!
#if 0
    glBindVertexArray(tessVAO);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(tess)/sizeof(tess[0]));
    glDrawArraysInstanced(GL_TRIANGLES, 0,
                          sizeof(tess)/sizeof(tess[0]),
                          /* world->hypercubeLocs.size() */1);
#endif

#if 1
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]), world->hypercubeLocs.size());
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
