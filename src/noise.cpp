#include "noise.h"
#include <stdlib.h>

// Something like:
// glActiveTexture(GL_TEXTURE0);
// should have been done by caller
NoiseTexture::NoiseTexture(uint32_t seed) {
  srand(seed);

  // Generate a bunch of values! (in [0, 1])
  for (int x=0; x<NOISE_WIDTH; x++) {
    for (int y=0; y<NOISE_HEIGHT; y++) {
      for (int z=0; z<NOISE_DEPTH; z++) {
        uint32_t rnum = rand();
        data[x * NOISE_WIDTH * NOISE_HEIGHT + y * NOISE_WIDTH + z] =
          ((float) rnum)/RAND_MAX;
      }
    }
  }

  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_3D, texId);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  glTexImage3D(GL_TEXTURE_3D,
               0,
               GL_RED,
               // 1, 1, 1,
               NOISE_WIDTH, NOISE_HEIGHT, NOISE_DEPTH,
               0,
               GL_RED,
               GL_FLOAT,
               data);
}

void NoiseTexture::bindToUniform(GLuint unit, GLuint programId, const char *uniformName) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glErrChk("1");
  glBindTexture(GL_TEXTURE_3D, texId);
  glErrChk("2");

  GLuint loc = glGetUniformLocation(programId, uniformName);
  glErrChk("3");
  glUniform1i(loc, unit);
  glErrChk("4");
  glBindSampler(unit, 0);
}
