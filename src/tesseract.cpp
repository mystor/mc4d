#include "tesseract.h"

#include "more.h"
#include "config.h"

#include <glm/glm.hpp>
#include <array>
#include <iostream>

using namespace glm;

// zero-out the faces array at program start
vec4 Tesseract::faces[Tesseract::FACE_COUNT * 4] = {};

// TODO(michael): This should actually probably make some effort to standardize the normals...
static void mk4DFace(vec4 fixed, vec4 free1, vec4 free2, vec4 *face) {
  // Get the fixed coordinates
  vec4 offset(0.5, 0.5, 0.5, 0.5);

  face[0] = fixed - offset;
  face[1] = fixed + free1 - offset;
  face[2] = fixed + free1 + free2 - offset;
  face[3] = fixed + free2 - offset;
}

void Tesseract::gen() {
  // These are some constant vectors in each of the directions which will make writing the gen code easier
  const vec4 ux(1, 0, 0, 0);
  const vec4 uy(0, 1, 0, 0);
  const vec4 uz(0, 0, 1, 0);
  const vec4 uw(0, 0, 0, 1);

  // We can generate the faces of the hypercube by fixing every pair of 2
  // of the 4 coordinates to each of 1 and 0.

  // There are 8 ways ways can choose two to fix (F = fixed, _ = free):
  // x y z w
  // F F _ _
  // F _ F _
  // F _ _ F
  // _ _ F F
  // _ F _ F
  // _ F F _

  vec4 *face_p = faces;
  for (float fst = 0; fst < 2; fst++) { // for fst in 0, 1
    for (float snd = 0; snd < 2; snd++) { // for snd in 0, 1
      // x y z w
      // F F _ _
      mk4DFace(fst*ux + snd*uy, uz, uw, face_p);
      face_p += 4;

      // F _ F _
      mk4DFace(fst*ux + snd*uz, uy, uw, face_p);
      face_p += 4;

      // F _ _ F
      mk4DFace(fst*ux + snd*uw, uy, uz, face_p);
      face_p += 4;

      // _ _ F F
      mk4DFace(fst*uz + snd*uw, ux, uy, face_p);
      face_p += 4;

      // _ F _ F
      mk4DFace(fst*uy + snd*uw, ux, uz, face_p);
      face_p += 4;

      // _ F F _
      mk4DFace(fst*uy + snd*uz, ux, uw, face_p);
      face_p += 4;
    }
  }

  if (Config::boolArg("print-face-verts")) {
    // Print out the faces
    for (size_t face=0; face<FACE_COUNT; face++) {
      std::cout << "("
                << faces[face * 4]
                << faces[face * 4 + 1]
                << faces[face * 4 + 2]
                << faces[face * 4 + 3]
                << ")\n";
    }
  }

  // Create the VAO

  GLuint VAO, VBO;

  /* int numExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

  for (int i=0; i<numExtensions; i++) {
    std::cout << glGetStringi(GL_EXTENSIONS, i) << std::endl;
  }
  std::cout << glGenVertexArrays << std::endl;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO); */
}
