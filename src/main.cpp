#include "tesseract.h"
#include "viewport.h"
#include "config.h"
#include "gpuProgram.h"
#include "world.h"
#include "project.h"
#include "blocktype.h"

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

  // Rotate automatically
  bool autorotXY, autorotXZ, autorotXW, autorotYZ, autorotYW, autorotZW;

  // Project using orthographic rather than projection
  // NOTE(michael): Disabled
  bool orthoProj;

  // Display the scene
  bool displayBlocks, displayWireframe;


  GLuint solidBlocksFB, waterBlocksFB;
  GLuint solidBlocksColorTex, waterBlocksColorTex;
  GLuint blocksDepthTex;
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
#if 0 // The wireframe projection doesn't work like this anymore due to multipass rendering
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
#endif
      // Enable automatic rotation
    case GLFW_KEY_Z: WS.autorotXY = !WS.autorotXY; break;
    case GLFW_KEY_X: WS.autorotXZ = !WS.autorotXZ; break;
    case GLFW_KEY_C: WS.autorotXW = !WS.autorotXW; break;
    case GLFW_KEY_V: WS.autorotYZ = !WS.autorotYZ; break;
    case GLFW_KEY_B: WS.autorotYW = !WS.autorotYW; break;
    case GLFW_KEY_N: WS.autorotZW = !WS.autorotZW; break;

    case GLFW_KEY_COMMA: WS.displayWireframe = !WS.displayWireframe; break;
    case GLFW_KEY_PERIOD: WS.displayBlocks = !WS.displayBlocks; break;

#if 0 // The orthoprojection looks really janky - disable it
    case GLFW_KEY_O: WS.orthoProj = !WS.orthoProj; break;
#endif
    }
  } else if (action == GLFW_RELEASE) {
    switch (key) {
    }
  }
}

static void resizeCallback(GLFWwindow *, int width, int height)
{
  glBindTexture(GL_TEXTURE_2D, WS.waterBlocksColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

  glBindTexture(GL_TEXTURE_2D, WS.solidBlocksColorTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

  glBindTexture(GL_TEXTURE_2D, WS.blocksDepthTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

  GL_ERR_CHK;
}

static void generateFrameBuffer(GLuint &color_tex, GLuint &depth_tex, GLuint &fb, int width, int height, bool genDepth)
{
  //RGBA8 2D texture, 24 bit depth texture, 256x256
  glGenTextures(1, &color_tex);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  GL_ERR_CHK;
  //NULL means reserve texture memory, but texels are undefined
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
  if (genDepth) {
    glGenTextures(1, &depth_tex);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    GL_ERR_CHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); GL_ERR_CHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); GL_ERR_CHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); GL_ERR_CHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); GL_ERR_CHK;
    // glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); GL_ERR_CHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE); GL_ERR_CHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); GL_ERR_CHK;
    //NULL means reserve texture memory, but texels are undefined
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
  }
  //-------------------------
  glGenFramebuffersEXT(1, &fb);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
  //Attach 2D texture to this FBO
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0/*mipmap level*/);
  //-------------------------
  //Attach depth texture to FBO
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0/*mipmap level*/);
  //-------------------------
  //Does the GPU support current FBO configuration?
  GLenum status;
  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status)
  {
  case GL_FRAMEBUFFER_COMPLETE_EXT:
    return;
  default:
    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT = " << GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT << "\n";
    std::cerr << "Status " << status << " when checking Framebuffer Status\n";
  }
  GL_ERR_CHK;
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
  glEnable(GL_LINE_SMOOTH);

  // Before we can create the world, we need to initialize the tesseract
  Tesseract::gen();
  GL_ERR_CHK;

  // The world is heap allocated because otherwise it will blow out the stack
  std::unique_ptr<World> world(new World());

  View view;

  // Get the maximum texture size
  GLint maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  std::cout << "Max texture size: " << maxTextureSize << std::endl;

  // Create the grass and stone blocks
  BlockType grassBlock(&world->grassLocs, 1);
  BlockType sandBlock(&world->sandLocs, 3);
  BlockType stoneBlock(&world->stoneLocs, 0);
  BlockType waterBlock(&world->waterLocs, 2);

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

  glm::vec4 linesVerts[Tesseract::LINES_SIZE];
  Tesseract::linesWithOffset(glm::vec4(0), linesVerts);
  GLuint linesVAO, linesVBO;
  glGenVertexArrays(1, &linesVAO);
  glBindVertexArray(linesVAO);

  // Create & Bind the linesVBO
  glGenBuffers(1, &linesVBO);
  glBindBuffer(GL_ARRAY_BUFFER, linesVBO);

  // Load the data
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(linesVerts),
               linesVerts,
               GL_STATIC_DRAW);
  GL_ERR_CHK;

  // Set up the vertex attrib array
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
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

  ShaderProgram blendShader;
  blendShader.createShader(GL_VERTEX_SHADER, blendvertGlsl);
  blendShader.createShader(GL_FRAGMENT_SHADER, blendfragGlsl);
  blendShader.link();
  GL_ERR_CHK;

  ShaderProgram wireShader;
  wireShader.createShader(GL_VERTEX_SHADER, wirevertGlsl);
  wireShader.createShader(GL_FRAGMENT_SHADER, wirefragGlsl);
  wireShader.link();
  GL_ERR_CHK;

  // Default world state values
  WS.viewAngle = 45;
  WS.up = glm::vec4(0, 1, 0, 0);
  WS.over = glm::vec4(0, 0, 1, 0);
  WS.eye = glm::vec4(-32, 0, 0, 0);
  WS.forward = normalize(glm::vec4(1, 0, 0, 0)); // normalize(-WS.eye);

  WS.displayBlocks = true;
  WS.displayWireframe = false;

  // Get the location of the uniforms on the GPU
  GLuint worldToEyeMat4DLoc = mainShader.uniformLocation("worldToEyeMat4D");
  GLuint recipTanViewAngleLoc = mainShader.uniformLocation("recipTanViewAngle");
  GLuint projMat3DLoc = mainShader.uniformLocation("projMat3D");
  GLuint eyeLoc = mainShader.uniformLocation("eye");
  GLuint forwardLoc = mainShader.uniformLocation("forward");
  GLuint hypercubeLoc = mainShader.uniformLocation("hypercube");
  GLuint hcCountLoc = mainShader.uniformLocation("hcCount");
  GLuint hcIndicatorLoc = mainShader.uniformLocation("hcIndicator");
  GLuint faceTexLoc = mainShader.uniformLocation("faceTex");
  GLuint srmLoc = mainShader.uniformLocation("srm");

  // Wireframe Shader locations
  GLuint wire_worldToEyeMat4DLoc = wireShader.uniformLocation("worldToEyeMat4D");
  GLuint wire_recipTanViewAngleLoc = wireShader.uniformLocation("recipTanViewAngle");
  GLuint wire_projMat3DLoc = wireShader.uniformLocation("projMat3D");
  GLuint wire_eyeLoc = wireShader.uniformLocation("eye");
  GLuint wire_forwardLoc = wireShader.uniformLocation("forward");
  GLuint wire_srmLoc = wireShader.uniformLocation("srm");

  // The textures for the blending shader
  GLuint solidTexLoc = blendShader.uniformLocation("solidTex");
  GLuint waterTexLoc = blendShader.uniformLocation("waterTex");

  GL_ERR_CHK;
  blendShader.activate();
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    generateFrameBuffer(WS.solidBlocksColorTex, WS.blocksDepthTex, WS.solidBlocksFB,
                        width, height, true);
    generateFrameBuffer(WS.waterBlocksColorTex, WS.blocksDepthTex, WS.waterBlocksFB,
                        width, height, false);

    glfwSetFramebufferSizeCallback(window, resizeCallback);
  }
  GL_ERR_CHK;

  mainShader.activate();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Set the texture up
  glUniform1i(faceTexLoc, 2);
  glActiveTexture(GL_TEXTURE0 + 2);
  glBindTexture(GL_TEXTURE_2D, faceTex);

#define DUMP(a) std::cout << #a << " = " << a.x << ",\t" << a.y << ",\t" << a.z << ",\t" << a.w << "\n"

  // Main loop
  double lastTime, thisTime, delta;

  // Rotation
  double rotXY = 0, rotXZ = 0, rotXW = 0;
  double rotYZ = 0, rotYW = 0, rotZW = 0;

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

    // World state updating */
    {
      const float SPEED = 1.00;
      // Get the component of forward perpendicular to up
      glm::vec4 perpForward = normalize(WS.forward - (glm::dot(WS.up, WS.forward) * WS.up));

#define ADJUST(uk, dk, var) if (glfwGetKey(window, GLFW_KEY_##uk)) {  \
        var += SPEED * delta; \
      }\
      if (glfwGetKey(window, GLFW_KEY_##dk)) {\
        var -= SPEED * delta;\
      }\
      if (WS.auto##var) {\
        var += SPEED * delta;\
      }

      ADJUST(Q, A, rotXY);
      ADJUST(W, S, rotXZ);
      ADJUST(E, D, rotXW);
      ADJUST(R, F, rotYZ);
      ADJUST(T, G, rotYW);
      ADJUST(Y, H, rotZW);
#undef ADJUST

      const float ZOOM_SPEED = 5.00;

      if (glfwGetKey(window, GLFW_KEY_EQUAL) && WS.eye.x < -16) {
        WS.eye.x += ZOOM_SPEED * delta;
      }
      if (glfwGetKey(window, GLFW_KEY_MINUS) && WS.eye.x > -50) {
        WS.eye.x -= ZOOM_SPEED * delta;
      }
    }

    // Calculate projecton stuff
    glm::mat4 worldToEyeMat4D = calcWorldToEyeMat4D(WS.up, WS.over, WS.forward);
    float invTanViewAngle = calcInvTanViewAngle(WS.viewAngle);

    // Value required for the 3D->2D projection
    glm::mat4 projMat3D = calcProjMat3D(WS.viewAngle, ratio, WS.orthoProj);

    // Create the scene rotation matrix
    glm::mat4 srm =
      glm::mat4(cosf(rotXY), sinf(rotXY), 0, 0,
                -sinf(rotXY), cosf(rotXY), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1) *
      glm::mat4(cosf(rotXZ), 0, sinf(rotXZ), 0,
                0, 1, 0, 0,
                -sinf(rotXZ), 0, cosf(rotXZ), 0,
                0, 0, 0, 1) *
      glm::mat4(cosf(rotXW), 0, 0, sinf(rotXW),
                0, 1, 0, 0,
                0, 0, 1, 0,
                -sinf(rotXW), 0, 0, cosf(rotXW)) *
      glm::mat4(1, 0, 0, 0,
                0, cosf(rotYZ), sinf(rotYZ), 0,
                0, -sinf(rotYZ), cosf(rotYZ), 0,
                0, 0, 0, 1) *
      glm::mat4(1, 0, 0, 0,
                0, cosf(rotYW), 0, sinf(rotYW),
                0, 0, 1, 0,
                0, -sinf(rotYW), 0, cosf(rotYW)) *
      glm::mat4(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, cosf(rotZW), sinf(rotZW),
                0, 0, -sinf(rotZW), cosf(rotZW));

    const size_t vaoSize = sizeof(verts)/sizeof(verts[0]);
    const size_t linesVaoSize = sizeof(linesVerts)/sizeof(linesVerts[0]);

    if (WS.displayBlocks) {
      mainShader.activate();
      // Sending data to the GPU
      glUniform4fv(eyeLoc, 1, glm::value_ptr(WS.eye)); GL_ERR_CHK;
      glUniform4fv(forwardLoc, 1, glm::value_ptr(WS.forward)); GL_ERR_CHK;
      glUniform1f(recipTanViewAngleLoc, invTanViewAngle); GL_ERR_CHK;
      glUniformMatrix4fv(worldToEyeMat4DLoc, 1, GL_FALSE, glm::value_ptr(worldToEyeMat4D)); GL_ERR_CHK;
      glUniformMatrix4fv(projMat3DLoc, 1, GL_FALSE, glm::value_ptr(projMat3D)); GL_ERR_CHK;
      glUniformMatrix4fv(srmLoc, 1, GL_FALSE, glm::value_ptr(srm)); GL_ERR_CHK;

      // Bind the tesseract VAO
      glBindVertexArray(VAO);

      // Render the solid geometry to a texture, storing the depth values in the depth texture
      // which is shared between the solid block frame buffer and the water frame buffer.
      mainShader.activate();
      glClearColor( 77.0/255, 219.0/255, 213.0/255, 1.0 );
      glBindFramebuffer(GL_FRAMEBUFFER, WS.solidBlocksFB);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      // Bind & draw the different block types
      grassBlock.bind(hypercubeLoc, hcCountLoc, hcIndicatorLoc);
      glDrawArraysInstanced(GL_TRIANGLES, 0, vaoSize, grassBlock.count);
      GL_ERR_CHK;

      sandBlock.bind(hypercubeLoc, hcCountLoc, hcIndicatorLoc);
      glDrawArraysInstanced(GL_TRIANGLES, 0, vaoSize, sandBlock.count);
      GL_ERR_CHK;

      stoneBlock.bind(hypercubeLoc, hcCountLoc, hcIndicatorLoc);
      glDrawArraysInstanced(GL_TRIANGLES, 0, vaoSize, stoneBlock.count);
      GL_ERR_CHK;

      // Render the water blocks to a texture, using the depth values from the rendering
      // of solid blocks to cull any obscured water surfaces.
      glBindFramebuffer(GL_FRAMEBUFFER, WS.waterBlocksFB);
      glClearColor(0,0,0,0);
      glClear( GL_COLOR_BUFFER_BIT ); // Don't clear depth, so we can use it to cull!

      waterBlock.bind(hypercubeLoc, hcCountLoc, hcIndicatorLoc);
      glDrawArraysInstanced(GL_TRIANGLES, 0, vaoSize, waterBlock.count);
      GL_ERR_CHK;

      // Render to the screen, binding the solid and water blocks to a shader, and blending
      // them together before outputting to the screen.
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClearColor(1, 0, 1, 1); // Magenta - useful error color
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

      blendShader.activate();
      // Bind the textures
      glActiveTexture(GL_TEXTURE0 + 4);
      glBindTexture(GL_TEXTURE_2D, WS.solidBlocksColorTex);
      glUniform1i(solidTexLoc, 4);
      glActiveTexture(GL_TEXTURE0 + 6);
      glBindTexture(GL_TEXTURE_2D, WS.waterBlocksColorTex);
      glUniform1i(waterTexLoc, 6);

      // Draw the viewport
      view.draw();
    } else {
      // Just clear the screen to the background sky color
      glClearColor( 77.0/255, 219.0/255, 213.0/255, 1.0 );
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    if (WS.displayWireframe) {
      wireShader.activate();
      // Send the data to the wireframe shader
      glUniform4fv(wire_eyeLoc, 1, glm::value_ptr(WS.eye)); GL_ERR_CHK;
      glUniform4fv(wire_forwardLoc, 1, glm::value_ptr(WS.forward)); GL_ERR_CHK;
      glUniform1f(wire_recipTanViewAngleLoc, invTanViewAngle); GL_ERR_CHK;
      glUniformMatrix4fv(wire_worldToEyeMat4DLoc, 1, GL_FALSE, glm::value_ptr(worldToEyeMat4D)); GL_ERR_CHK;
      glUniformMatrix4fv(wire_projMat3DLoc, 1, GL_FALSE, glm::value_ptr(projMat3D)); GL_ERR_CHK;
      glUniformMatrix4fv(wire_srmLoc, 1, GL_FALSE, glm::value_ptr(srm)); GL_ERR_CHK;

      // Get rid of the depth buffer so it renders in front
      glClear( GL_DEPTH_BUFFER_BIT );

      // Draw away!
      glBindVertexArray(linesVAO);
      glDrawArrays(GL_LINES, 0, linesVaoSize);
      GL_ERR_CHK;
    }


    // Swap and poll events
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
