#include "controls.h"
// #include "tesseract.h"
#include "config.h"
#include "gpuProgram.h"
#include "noise.h"

#include "fragShader.h"
#include "vertShader.h"

#include "gl.h"

#include <stdlib.h>
#include <iostream>

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

  for (size_t i=0; i<sizeof(worldTex.data)/sizeof(worldTex.data[0]); i++) {
    std::cout << worldTex.data[i] << std::endl;
  }

  std::cout << worldTex.texId;
  // GLuint worldLoc = glGetUniformLocation(gp.program_id, "world");
  // glBindSampler(0, worldTex.texId);

  worldTex.bindToUniform(0, gp.program_id, "world");

  glErrChk("After WorldTex");

  /* GLuint worldLoc = glGetUniformLocation(gp.program_id, "world");

  // Create the world texture (janky!)
  GLuint texId;
  float world[] = {1};
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_3D, texId);
  glTexImage3D(GL_TEXTURE_3D,
               0,
               GL_RED, // Only set the red component
               1,1,1, // Dimensions
               0,
               GL_RED,
               GL_FLOAT,
               world);

  // Connect the two together
  glBindSampler(0, texId);
  glUniform1i(worldLoc, 0); */

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
  }

  // Exit the program
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
