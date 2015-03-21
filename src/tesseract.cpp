#include "tesseract.h"

#include "more.h"
#include "config.h"

#include <array>
#include <iostream>

using namespace glm;

// zero-out the faces array at program start
TesseractVert Tesseract::faces[Tesseract::FACE_COUNT * Tesseract::FACE_SIZE] = {};

// TODO(michael): This should actually probably make some effort to standardize the normals...
static void mk4DFace(vec4 fixed, vec4 free1, vec4 free2, TesseractVert *face) {
  // Get the fixed coordinates
  vec4 offset(0.5, 0.5, 0.5, 0.5);

  face[0] = fixed - offset;
  face[0].color = vec4(0, 0, 0, 0);
  face[1] = fixed + free1 - offset;
  face[1].color = vec4(0, 1, 0, 0);
  face[2] = fixed + free1 + free2 - offset;
  face[2].color = vec4(1, 1, 0, 0);
  face[3] = fixed + free2 - offset;
  face[3].color = vec4(1, 0, 0, 0);
}

static void mk4DCube(vec4 fixed, vec4 free1, vec4 free2, vec4 free3, TesseractVert *face) {
  mk4DFace(fixed + free1, free2, free3, face);
  face += 4;
  mk4DFace(fixed, free2, free3, face);
  face += 4;
  mk4DFace(fixed + free2, free1, free3, face);
  face += 4;
  mk4DFace(fixed, free1, free3, face);
  face += 4;
  mk4DFace(fixed + free3, free1, free2, face);
  face += 4;
  mk4DFace(fixed, free1, free2, face);
}

void Tesseract::gen() {
  // These are some constant vectors in each of the directions which will
  // make writing the gen code easier
  const vec4 ux(1, 0, 0, 0);
  const vec4 uy(0, 1, 0, 0);
  const vec4 uz(0, 0, 1, 0);
  const vec4 uw(0, 0, 0, 1);

#ifdef TESSERACT_CUBES
  // Create 8 cubes!
  {
    TesseractVert *face_p = faces;

#define MKFACES(a, b, c, d) mk4DCube(vec4(), b, c, d, face_p);\
    face_p += 24;\
    mk4DCube(a, b, c, d, face_p);\
    face_p += 24

    MKFACES(ux, uy, uz, uw);
    MKFACES(uy, ux, uz, uw);
    MKFACES(uz, ux, uy, uw);
    MKFACES(uw, ux, uy, uz);
#undef MKFACES
  }

#else
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

  TesseractVert *face_p = faces;
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
#endif

#if 0
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
#endif
}

void Tesseract::linesWithOffset(glm::vec4 offset, glm::vec4 *out) {
  for (size_t face=0; face<FACE_COUNT; face++) {
    // Create the verts
    out[face * 8 + 0] = faces[face * 4 + 0].loc + offset;
    out[face * 8 + 1] = faces[face * 4 + 1].loc + offset;

    out[face * 8 + 2] = faces[face * 4 + 1].loc + offset;
    out[face * 8 + 3] = faces[face * 4 + 2].loc + offset;

    out[face * 8 + 4] = faces[face * 4 + 2].loc + offset;
    out[face * 8 + 5] = faces[face * 4 + 3].loc + offset;

    out[face * 8 + 6] = faces[face * 4 + 3].loc + offset;
    out[face * 8 + 7] = faces[face * 4 + 0].loc + offset;
  }
}

void Tesseract::withOffset(glm::vec4 offset, TesseractVert *out) {
  for (size_t face=0; face<FACE_COUNT; face++) {
    // First triangle
    out[face * 6 + 0] = faces[face * 4 + 0] + offset;
    out[face * 6 + 1] = faces[face * 4 + 1] + offset;
    out[face * 6 + 2] = faces[face * 4 + 2] + offset;

    // Second Triangle
    out[face * 6 + 3] = faces[face * 4 + 2] + offset;
    out[face * 6 + 4] = faces[face * 4 + 3] + offset;
    out[face * 6 + 5] = faces[face * 4 + 0] + offset;
  }
}
