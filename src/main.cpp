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

#define dumpVec4(v4) std::cout << v4.x << ", "\
  << v4.y << ", "\
  << v4.z << ", "\
  << v4.w << "\n";

// Fills path with count cells in 4d space which are touched by the vector starting
// at start and passing in the direction dir
// Based on http://stackoverflow.com/questions/11694886/traverse-a-2-5d-grid
void gridIntersectPoints(vec4 start, vec4 dir, ivec4 *path, size_t count) {
  ivec4 dirSigns(dir.x < 0 ? -1 : 1,
                 dir.y < 0 ? -1 : 1,
                 dir.z < 0 ? -1 : 1,
                 dir.w < 0 ? -1 : 1);

  vec4 pos = start;

  while (count--) {
    // Get the current cell
    ivec4 posInt(pos);
    *path++ = posInt;

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



  /* float maxDir = max(max(dir.x, dir.y),
                     max(dir.z, dir.w));

  vec4 sdir = dir/maxDir;
  dumpVec4(dir);

  vec4 pos = start;

  while (count--) {
    *path++ = pos;
    pos += sdir;
    } */

  /* const float gridResolution = 1;

  dir = normalize(dir);

  dumpVec4(dir)

  vec4 delta(gridResolution/fabs(dir.x),
             gridResolution/fabs(dir.y),
             gridResolution/fabs(dir.z),
             gridResolution/fabs(dir.w));

  dumpVec4(delta);

  // Truncate and scale
  ivec4 startGrid = start / gridResolution;
  ivec4 currentGrid = startGrid;

  dumpVec4(currentGrid);

  // The step amount
  ivec4 step(dir.x<0 ? -1 : 1,
             dir.y<0 ? -1 : 1,
             dir.z<0 ? -1 : 1,
             dir.w<0 ? -1 : 1);

  // TODO(michael): What?
  vec4 cd(((step.x>0?start.x:start.x+1)*gridResolution-start.x)/dir.x,
          ((step.y>0?start.y:start.y+1)*gridResolution-start.y)/dir.y,
          ((step.z>0?start.z:start.z+1)*gridResolution-start.z)/dir.z,
          ((step.w>0?start.w:start.w+1)*gridResolution-start.w)/dir.w);

  std::cout << (step.z>0?start.z:start.z+1)*gridResolution-start.z;

  std::cout << cd.x << ", "
            << cd.y << ", "
            << cd.z << ", "
            << cd.w << "\n";

  while (count--) {
    *path = currentGrid;
    path++;

    if (cd.x < cd.y && cd.x < cd.z && cd.x < cd.w) {
      cd.x += delta.x;
      currentGrid.x += step.x;
    } else if (cd.y < cd.z && cd.y < cd.w) {
      cd.y += delta.y;
      currentGrid.y += step.y;
    } else if (cd.z < cd.w) {
      cd.z += delta.z;
      currentGrid.z += step.z;
    } else {
      cd.w += delta.w;
      currentGrid.w += step.w;
    }
    } */
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

  for (size_t i=0; i<sizeof(worldTex.data)/sizeof(worldTex.data[0]); i++) {
    std::cout << worldTex.data[i] << std::endl;
  }

  std::cout << worldTex.texId;
  // GLuint worldLoc = glGetUniformLocation(gp.program_id, "world");
  // glBindSampler(0, worldTex.texId);

  worldTex.bindToUniform(0, gp.program_id, "world");

  glErrChk("After WorldTex");


  // ___ TEST___
  std::cout << "HERE\n";
  ivec4 gips[10];
  gridIntersectPoints(vec4(0,0,0,0), vec4(0.5,1,0,0), gips, 10);
  for (size_t i=0; i<10; i++) {
    std::cout << gips[i].x << ", "
              << gips[i].y << ", "
              << gips[i].z << ", "
              << gips[i].w << "\n";
  }
  std::cout << "THERE\n";

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
