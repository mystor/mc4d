#include "world.h"
#include "noise.h"

/* static const int32_t NOISE_WIDTH = 16;
static const int32_t NOISE_HEIGHT = 16;
static const int32_t NOISE_DEPTH = 16;
static const int32_t NOISE_SPAN = 16; */


static const glm::ivec4 WORLD_DIM(128, 64, 128, 128);

static inline double worldSample(glm::dvec4 loc) {
  double rawSimplex = simplexNoise4D(glm::dvec4(1, 1, 1, 1) + // This is kinda like a seed?
                                     (loc / glm::dvec4(WORLD_DIM)));

  return rawSimplex + loc.y/WORLD_DIM.y;
}

World::World() {
  std::cout << worldSample(glm::ivec4(0, 0, 0, 0)) << "\n";

  size_t vertCount = 0;
  size_t elemCount = 0;

  // Ensure that the perm matrix is initialized
  initPerm();
  // Loop over the possible things in the world
  int32_t x, y, z, w;
  for (x=0; x<WORLD_DIM.x; x++) {
    for (y=0; y<WORLD_DIM.y; y++) {
      for (z=0; z<WORLD_DIM.z; z++) {
        for (w=0; w<WORLD_DIM.w; w++) {
          double sample = worldSample(glm::dvec4(x, y, z, w));

          if (sample < 0) {
            // Check if the other faces are also hits
            if (worldSample(glm::dvec4(x-1, y, z, w)) < 0 &&
                worldSample(glm::dvec4(x+1, y, z, w)) < 0 &&
                worldSample(glm::dvec4(x, y-1, z, w)) < 0 &&
                worldSample(glm::dvec4(x, y+1, z, w)) < 0 &&
                worldSample(glm::dvec4(x, y, z-1, w)) < 0 &&
                worldSample(glm::dvec4(x, y, z+1, w)) < 0 &&
                worldSample(glm::dvec4(x, y, z, w-1)) < 0 &&
                worldSample(glm::dvec4(x, y, z, w+1)) < 0) {
              // SKIP!
            } else {
              vertCount += 16;
              elemCount += 24*4;
            }
          } else {
            // AIR
            // DO NOTHING
          }
        }
      }
    }
    std::cout << ".\n";
  }

  std::cout << "vertCount = " << vertCount << "\n";
  std::cout << "elemCount = " << elemCount << "\n";
}
