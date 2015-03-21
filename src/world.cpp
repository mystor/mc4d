#include "world.h"
#include "noise.h"

static inline double noiseSample(glm::dvec4 loc) {
  double rawSimplex = simplexNoise4D(glm::dvec4(1, 1, 1, 1) + // This is kinda like a seed?
                                     (loc / glm::dvec4(WORLD_DIM)));

  return rawSimplex + loc.y/WORLD_DIM.y;
}


HyperCubeTypes World::worldSample(int32_t x, int32_t y, int32_t z, int32_t w) {
  if (x < 0 || y < 0 || z < 0 || w < 0) {
    return HCT_STONE;
  } else if (x >= WD_X || y >= WD_Y || z >= WD_Z || w >= WD_W) {
    return HCT_STONE;
  } else {
    return hypercubes[x][y][z][w];
  }
}

World::World() {
  std::cout << noiseSample(glm::ivec4(0, 0, 0, 0)) << "\n";

  // Ensure that the perm matrix is initialized
  initPerm();
  // Loop over the possible things in the world
  std::cout << "Starting world generation" << std::endl;;
  int32_t x, y, z, w;
  for (x=0; x<WORLD_DIM.x; x++) {
    for (y=0; y<WORLD_DIM.y; y++) {
      for (z=0; z<WORLD_DIM.z; z++) {
        for (w=0; w<WORLD_DIM.w; w++) {
          double sample = noiseSample(glm::dvec4(x, y, z, w));

          if (sample < 0.5) {
            hypercubes[x][y][z][w] = HCT_STONE;
          } else {
            hypercubes[x][y][z][w] = HCT_AIR;
          }
        }
      }
    }
    std::cout << x << "/" << WORLD_DIM.x << "\n";
  }
  std::cout << "Done world generation" << std::endl;;

  std::cout << "Starting mesh generation" << std::endl;

  for (x=0; x<WORLD_DIM.x; x++) {
    for (y=0; y<WORLD_DIM.y; y++) {
      for (z=0; z<WORLD_DIM.z; z++) {
        for (w=0; w<WORLD_DIM.w; w++) {
          HyperCubeTypes hct = hypercubes[x][y][z][w];

          switch (hct) {
          case HCT_AIR:
            break;
          case HCT_STONE:
            if (worldSample(x-1, y, z, w) != HCT_AIR &&
                worldSample(x+1, y, z, w) != HCT_AIR &&
                worldSample(x, y-1, z, w) != HCT_AIR &&
                worldSample(x, y+1, z, w) != HCT_AIR &&
                worldSample(x, y, z-1, w) != HCT_AIR &&
                worldSample(x, y, z+1, w) != HCT_AIR &&
                worldSample(x, y, z, w-1) != HCT_AIR &&
                worldSample(x, y, z, w+1) != HCT_AIR) {
              // All surrounding cubes are filled
            } else {
              hypercubeLocs.push_back(glm::vec4(x, y, z, w));
            }
            break;
          }
        }
      }
    }
    std::cout << x << "/" << WORLD_DIM.x << "\n";
  }
  std::cout << "Done mesh generation" << std::endl;

}

void World::draw() {
  std::cerr << "UNIMPLEMENTED\n";
  exit(-1);
}
