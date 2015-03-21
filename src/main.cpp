// #include "controls.h"
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

// The state of the world.
static struct WorldState {
  // Camera position and other info
  // This is used to derive the matrices
  float viewAngle;
  glm::vec4 up;
  glm::vec4 over;
  glm::vec4 eye;
  glm::vec4 forward;
} WS;

// If GLFW reports an error, this will be called
static void errorCallback(int error, const char* description)
{
  std::cerr << "GLFW ERROR: " << error << ": " << description << std::endl;
  fputs(description, stderr);
}

// Keyboard handler
static void keyCallback(GLFWwindow* window, int key,
                        int /* scancode */, int action, int /* mods */)
{
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_ESCAPE: {
      glfwSetWindowShouldClose(window, GL_TRUE);
    } break;
    case GLFW_KEY_M: {
      static bool lines = false;
      if (lines) {
        lines = !lines;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      } else {
        lines = !lines;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
    } break;
    }
  } else if (action == GLFW_RELEASE) {
    switch (key) {
    }
  }
}

int main(int argc, char **argv)
{
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

  // Before we can create the world, we need to initialize the tesseract
  Tesseract::gen();
  GL_ERR_CHK;

  // The world is heap allocated because otherwise it will blow out the stack
  std::unique_ptr<World> world(new World());
  std::cout << "Drawing " << world->hypercubeLocs.size() << " hypercubes." << std::endl;

  // Check the maximum texture size on this computer
  GLint maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  std::cout << "Max texture size: " << maxTextureSize << std::endl;
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

  // Noise generation for surfaces. Done using random noise
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

  // Get the verts for a tesseract
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

  // Set up the vertex attrib array
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 32, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 32, (void*) 16);
  GL_ERR_CHK;

  // Create an initialize the GPU program which does basically all
  // of the actual rendering for the program
  ShaderProgram mainShader;
  mainShader.createShader(GL_VERTEX_SHADER, vertGlsl);
  mainShader.createShader(GL_FRAGMENT_SHADER, fragGlsl);
  mainShader.createShader(GL_GEOMETRY_SHADER, geomGlsl);
  mainShader.link();
  mainShader.activate();
  GL_ERR_CHK;

  WS.viewAngle = 45;
  WS.up = glm::vec4(0, 1, 0, 0);
  WS.over = glm::vec4(0, 0, 1, 0);
  WS.eye = glm::vec4(-32, 8, 8, 8);
  WS.forward = glm::vec4(1, 0, 0, 0); // normalize(-WS.eye);

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

  // Main loop
  double lastTime, thisTime, delta;
  double mouseX = -1, mouseY = -1;
  lastTime = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    // Calculate the time delta
    thisTime = glfwGetTime();
    delta = thisTime - lastTime;
    lastTime = thisTime;

    // Get the screen aspect ratio
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    // Clear the color for the background
    glClearColor( 77.0/255, 219.0/255, 213.0/255, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Mouse Movement
    // Determine t

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    if (abs(cursorX - mouseX) > 1 || abs(cursorY != mouseY) > 1) {
      std::cout << mouseX - cursorX << " " << mouseY - cursorY << "\n";

      int windowWidth, windowHeight;
      glfwGetWindowSize(window, &windowWidth, &windowHeight);
      glm::vec2 cent(windowWidth/2, windowHeight/2.0);
      glfwSetCursorPos(window, cent.x, cent.y);

      mouseX = cent.x, mouseY = cent.y;
    }
    // World state updating
    {
      const float SPEED = 0.25;
      // Get the component of forward perpendicular to up
      glm::vec4 perpForward = normalize(WS.forward - glm::dot(WS.up, WS.forward));

      // Move!
      if (glfwGetKey(window, GLFW_KEY_W)) {
        WS.eye += perpForward * SPEED;
      }
      if (glfwGetKey(window, GLFW_KEY_A)) {
        WS.eye += WS.over * SPEED;
      }
      if (glfwGetKey(window, GLFW_KEY_S)) {
        WS.eye -= perpForward * SPEED;
      }
      if (glfwGetKey(window, GLFW_KEY_D)) {
        WS.eye -= WS.over * SPEED;
      }
      if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        WS.eye += WS.up * SPEED;
      }
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
        WS.eye -= WS.up * SPEED;
      }
    }

    // Calculate projecton stuff
    glm::mat4 worldToEyeMat4D = calcWorldToEyeMat4D(WS.up, WS.over, WS.forward);
    float invTanViewAngle = calcInvTanViewAngle(WS.viewAngle);

    // Value required for the 3D->2D projection
    glm::mat4 projMat3D = calcProjMat3D(WS.viewAngle, ratio);

    /*
ROTATION MATRIX:
glm::mat4(cosf(R), 0, 0, -sinf(R),
          0, 1, 0, 0,
          0, 0, 1, 0,
          sinf(R), 0, 0, cosf(R))
    */

    // Sending data to the GPU
    glUniform4fv(eyeLoc, 1, glm::value_ptr(WS.eye)); GL_ERR_CHK;
    glUniform1f(recipTanViewAngleLoc, invTanViewAngle); GL_ERR_CHK;
    glUniformMatrix4fv(worldToEyeMat4DLoc, 1, GL_FALSE, glm::value_ptr(worldToEyeMat4D)); GL_ERR_CHK;
    glUniformMatrix4fv(projMat3DLoc, 1, GL_FALSE, glm::value_ptr(projMat3D)); GL_ERR_CHK;

    // Draw the tesseracts!
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]), world->hypercubeLocs.size());
    GL_ERR_CHK;

    // world->draw();

    glfwSwapBuffers(window);
    glfwPollEvents();

    GL_ERR_CHK;
    calcFPS();
  }

  // Exit the program
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
