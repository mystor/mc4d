#include "controls.h"
// #include "tesseract.h"
#include "config.h"
#include "gpuProgram.h"
#include "noise.h"

#include "fragShader.h"
#include "vertShader.h"

#include "gl.h"
#include <glm/glm.hpp>

#include <stdlib.h>
#include <iostream>

using namespace glm;

// This is a really simple FPS thing from http://r3dux.org/2012/07/a-simple-glfw-fps-counter/
double calcFPS(double theTimeInterval = 1.0) {
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



#define dumpVec4(v4) std::cout << v4.x << ", "\
  << v4.y << ", "\
  << v4.z << ", "\
  << v4.w << "\n";

// This code will be ported to glsl, but I'm verifying that the algo works correctly in
// c++ first

static uint32_t count = 0;
bool gridIntersectPointHandler(ivec4 cell) {
  count++;
  if (count > 10) return true;
  dumpVec4(cell);
  return false;
}

// Fills path with count cells in 4d space which are touched by the vector starting
// at start and passing in the direction dir
void gridIntersectPoints(vec4 start, vec4 dir) {
  ivec4 dirSigns(dir.x < 0 ? -1 : 1,
                 dir.y < 0 ? -1 : 1,
                 dir.z < 0 ? -1 : 1,
                 dir.w < 0 ? -1 : 1);

  vec4 pos = start;

  while (true) {
    // Get the current cell
    ivec4 posInt(pos);
    if (gridIntersectPointHandler(posInt))
      break;

    // The value gotten by moving to one block further in each dimension
    vec4 targets = posInt + dirSigns;
    // The distance which must be traveled in each direction to get this
    vec4 deltas = targets - pos;
    // How many times dir must be traveled to cover this distance
    vec4 deltaInDirs = deltas / dir;

    // Choose the smallest one
    float minDeltaInDirs = min(min(deltaInDirs.x, deltaInDirs.y),
                               min(deltaInDirs.z, deltaInDirs.w));

    // A small fudge to make sure that the next block is actually entred
    // in case floating point errors cause the same block to be tested repeatedly
    minDeltaInDirs += 1.0/16.0;

    // Move along minDeltaInDirs!
    pos += minDeltaInDirs * dir;
  }

}

bool gipHandler(vec4 pt, vec4 norm)
{
  std::cout << "gipHandler: ";
  dumpVec4(pt);
  dumpVec4(norm);
  std::cout << std::endl;
  return false;
}

void gip(vec4 start, vec4 dir)
{
  // TODO(michael):
  // This is being used instead of sign() because sign(0) = 0, which means that
  // we could get a NaN for a deltaInDirs argument, which will totally bugger everything up
  vec4 dirSigns = vec4(dir.x < 0 ? -1 : 1,
                       dir.y < 0 ? -1 : 1,
                       dir.z < 0 ? -1 : 1,
                       dir.w < 0 ? -1 : 1);
  // vec4 dirSigns = sign(dir);

  vec4 pos = start;

  int remaining = 10;
  while (remaining-- > 0) { // Clamp the max # of things to look up (max view distance)
    vec4 posInt = floor(pos); // Round the coordinate down to the closest block
    /* if (gipHandler(posInt)) { // Check if the block should count as a hit
      break;
      } */

    // The value gotten by moving to one block further in each dimension
    vec4 targets = posInt + dirSigns;
    // The distance which must be traveled in each direction to get this
    vec4 deltas = targets - pos;
    // How many times dir must be traveled to cover this distance
    vec4 deltaInDirs = deltas / dir;

    // TODO(michael): Make this less terrible
    // Choose the smallest one
    float minDeltaInDirs = min(min(deltaInDirs.x, deltaInDirs.y),
                               min(deltaInDirs.z, deltaInDirs.w));

    // A small fudge to make sure that the next block is actually entred
    // in case floating point errors cause the same block to be tested repeatedly
    // minDeltaInDirs += 1.0/16.0;

    // Move along minDeltaInDirs!
    pos += minDeltaInDirs * dir;

    vec4 newPosInt = floor(pos); // Round the coordinate down to the closest block
    vec4 normal = newPosInt - posInt;
    if (gipHandler(newPosInt, normal)) { // Check if the block should count as a hit
      break;
    }
  }
}

struct View {
  GLuint VAO;
  uint32_t size;

  View()
  {
    float verts[] = {
      -1, 1,
      1, 1,
      1, -1,
      -1, -1
    };

    GLuint VBO;

    // Create & Bind the VAO
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    size = sizeof(verts)/sizeof(verts[0]);
    // View view = { VAO, sizeof(verts)/sizeof(verts[0]) };
    // return view;
  }

  // Draws the view represented by the VAO
  void draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, size);
  }
};

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

  View view = View();

  glErrChk("Before GP");
  // Create an initialize the GPU program which does basically all
  // of the actual rendering for the program
  GPUProgram gp;
  gp.init(vertGlsl, fragGlsl);
  gp.activate();
  glErrChk("After GPUProgram");

  glActiveTexture(GL_TEXTURE0);
  NoiseTexture worldTex(0); // TODO(michael): Find a good seed
  glErrChk("BeforeBInd");

  worldTex.bindToUniform(0, gp.program_id, "world");
  glErrChk("After WorldTex");

  // ___ TEST___
  std::cout << "HERE\n";
  // gridIntersectPoints(vec4(0,0,0,0), vec4(0.5,1,0,0));
  gip(vec4(0,0,0,0), vec4(-0.4,0.3,0,0));
  std::cout << "THERE\n";

  // Set the eye, forward, up, and right vectors
  GLuint eyeLoc = glGetUniformLocation(gp.program_id, "eye");
  GLuint forwardLoc = glGetUniformLocation(gp.program_id, "forward");
  GLuint upLoc = glGetUniformLocation(gp.program_id, "up");
  GLuint rightLoc = glGetUniformLocation(gp.program_id, "right");

  // These are the defaults for right now
  glUniform3f(eyeLoc, 64, 32, 0);
  glUniform3f(forwardLoc, 1, 0, 0);
  glUniform3f(upLoc, 0, 1, 0);
  glUniform3f(rightLoc, 0, 0, 1);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // TODO(michael): Is this necessary?
    glClearColor( 0.0, 0.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT );

    // Draw the view
    view.draw();

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
