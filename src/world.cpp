#include "world.h"
#include "noise.h"

static inline double noiseSample(glm::dvec4 loc) {
  double rawSimplex = simplexNoise4D(glm::dvec4(1, 1, 1, 1) + // This is kinda like a seed?
                                     (loc / glm::dvec4(WORLD_DIM)));

  return rawSimplex + loc.y/WORLD_DIM.y;
}


HyperCubeTypes World::worldSample(int32_t x, int32_t y, int32_t z, int32_t w) {
  if (x < 0 || y < 0 || z < 0 || w < 0) {
    return HCT_AIR;// TONE;
  } else if (x >= WD_X || y >= WD_Y || z >= WD_Z || w >= WD_W) {
    return HCT_AIR;
  } else {
    return hypercubes[x][y][z][w];
  }
}

World::World() {
  std::cout << noiseSample(glm::ivec4(0, 0, 0, 0)) << "\n";

  // Ensure that the perm matrix is initialized
  initPerm();
  // Loop over the possible things in the world
  std::cout << "Starting world generation" << std::endl;
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
  std::cout << "Done world generation" << std::endl;

  std::cout << "Growing grass" << std::endl;
  for (x=0; x<WORLD_DIM.x; x++) {
    for (y=0; y<WORLD_DIM.y; y++) {
      for (z=0; z<WORLD_DIM.z; z++) {
        for (w=0; w<WORLD_DIM.w; w++) {
          if (worldSample(x, y+1, z, w) == HCT_AIR && hypercubes[x][y][z][w] == HCT_STONE) {
            if (y >= WORLD_DIM.y / 2) {
              hypercubes[x][y][z][w] = HCT_GRASS;
            } else {
              hypercubes[x][y][z][w] = HCT_SAND;
            }
          }
        }
      }
    }
    std::cout << x << "/" << WORLD_DIM.x << "\n";
  }
  std::cout << "Done growing grass" << std::endl;

  std::cout << "Filling ponds" << std::endl;
  for (x=0; x<WORLD_DIM.x; x++) {
    for (y=0; y<WORLD_DIM.y / 2; y++) { // Only half filled with water
      for (z=0; z<WORLD_DIM.z; z++) {
        for (w=0; w<WORLD_DIM.w; w++) {
          if (hypercubes[x][y][z][w] == HCT_AIR) {
            hypercubes[x][y][z][w] = HCT_WATER;
          }
        }
      }
    }
    std::cout << x << "/" << WORLD_DIM.x << "\n";
  }
  std::cout << "Done filling ponds" << std::endl;

  std::cout << "Starting mesh generation" << std::endl;

  for (x=0; x<WORLD_DIM.x; x++) {
    for (y=0; y<WORLD_DIM.y; y++) {
      for (z=0; z<WORLD_DIM.z; z++) {
        for (w=0; w<WORLD_DIM.w; w++) {
          HyperCubeTypes hct = hypercubes[x][y][z][w];

#define SURROUNDED (worldSample(x-1, y, z, w) >= HCT_SOLID_START && \
                    worldSample(x+1, y, z, w) >= HCT_SOLID_START && \
                    worldSample(x, y-1, z, w) >= HCT_SOLID_START && \
                    worldSample(x, y+1, z, w) >= HCT_SOLID_START && \
                    worldSample(x, y, z-1, w) >= HCT_SOLID_START && \
                    worldSample(x, y, z+1, w) >= HCT_SOLID_START && \
                    worldSample(x, y, z, w-1) >= HCT_SOLID_START && \
                    worldSample(x, y, z, w+1) >= HCT_SOLID_START)

          switch (hct) {
          case HCT_AIR:
            break;
          case HCT_GRASS:
            if (!SURROUNDED) {
              grassLocs.push_back(glm::vec4(x, y, z, w));
            }
            break;
          case HCT_SAND:
            if (!SURROUNDED) {
              sandLocs.push_back(glm::vec4(x, y, z, w));
            }
            break;
          case HCT_STONE:
            if (!SURROUNDED) {
              stoneLocs.push_back(glm::vec4(x, y, z, w));
            }
            break;
          case HCT_WATER:
            if (!SURROUNDED) {
              waterLocs.push_back(glm::vec4(x, y, z, w));
            }
            break;
          default:
            std::cerr << "INVALID BLOCK TYPE " << hct << "\n";
            exit(-1);
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
