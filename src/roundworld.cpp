#include "roundworld.h"
#include "noise.h"

#define DUMP(a) std::cout << #a << " = " << a.x << ",\t" << a.y << ",\t" << a.z << ",\t" << a.w << "\n"

static inline double baseNoiseSample(glm::dvec4 loc) {
  double rawSimplex = simplexNoise4D(glm::dvec4(1, 1, 1, 1) + // This is kinda like a seed?
                                     5.0 * (loc / glm::dvec4(ROUNDWORLD_DIM)));

  return rawSimplex;
}

static inline double cloudNoiseSample(glm::dvec4 loc) {
  double rawSimplex = simplexNoise4D(glm::dvec4(1, 1, 1, 1) + // This is kinda like a seed?
                                     3.0 * (loc / glm::dvec4(ROUNDWORLD_DIM)));

  return rawSimplex;
}

static inline double noiseSample(glm::dvec4 loc, double rawSimplex) {
  glm::dvec4 fromCenter = loc - (glm::dvec4(ROUNDWORLD_DIM) / 2.0);

  return (0.20 * rawSimplex) + (length(fromCenter) / 12);
}

HyperCubeTypes RoundWorld::worldSample(int32_t x, int32_t y, int32_t z, int32_t w) {
  if (x < 0 || y < 0 || z < 0 || w < 0) {
    return HCT_AIR;// TONE;
  } else if (x >= RWD_X || y >= RWD_Y || z >= RWD_Z || w >= RWD_W) {
    return HCT_AIR;
  } else {
    return hypercubes[x][y][z][w];
  }
}

RoundWorld::RoundWorld() {
  // std::cout << noiseSample(glm::ivec4(0, 0, 0, 0)) << "\n";

  // Ensure that the perm matrix is initialized
  initPerm();
  // Loop over the possible things in the world
  std::cout << "Starting world generation" << std::endl;
  int32_t x, y, z, w;
  for (x=0; x<ROUNDWORLD_DIM.x; x++) {
    for (y=0; y<ROUNDWORLD_DIM.y; y++) {
      for (z=0; z<ROUNDWORLD_DIM.z; z++) {
        for (w=0; w<ROUNDWORLD_DIM.w; w++) {
          glm::dvec4 loc(x, y, z, w);
          double baseSample = baseNoiseSample(loc);
          double cloudSample = cloudNoiseSample(loc);
          double sample = noiseSample(loc, baseSample);
          glm::dvec4 fromCenter = loc - (glm::dvec4(ROUNDWORLD_DIM) / 2.0);
          double rad = length(fromCenter);

          if (sample < 0.6) {
            hypercubes[x][y][z][w] = HCT_STONE;
          } else if (sample < 0.7) {
            hypercubes[x][y][z][w] = HCT_GRASS;
          } else if (rad > 13 && rad < 15 && cloudSample > 0.8) {
            hypercubes[x][y][z][w] = HCT_CLOUD;
          } else {
            hypercubes[x][y][z][w] = HCT_AIR;
          }
        }
      }
    }
    std::cout << x << "/" << ROUNDWORLD_DIM.x << "\n";
  }
  std::cout << "Done world generation" << std::endl;

  std::cout << "Growing grass" << std::endl;
  for (x=0; x<ROUNDWORLD_DIM.x; x++) {
    for (y=0; y<ROUNDWORLD_DIM.y; y++) {
      for (z=0; z<ROUNDWORLD_DIM.z; z++) {
        for (w=0; w<ROUNDWORLD_DIM.w; w++) {
          if (worldSample(x, y+1, z, w) == HCT_AIR && hypercubes[x][y][z][w] == HCT_STONE) {
            /* if (y >= ROUNDWORLD_DIM.y / 2) {
              hypercubes[x][y][z][w] = HCT_GRASS;
            } else {
              hypercubes[x][y][z][w] = HCT_SAND;
              } */
          }
        }
      }
    }
    std::cout << x << "/" << ROUNDWORLD_DIM.x << "\n";
  }
  std::cout << "Done growing grass" << std::endl;

  std::cout << "Filling ponds" << std::endl;
  srand(124);
  for (x=0; x<ROUNDWORLD_DIM.x; x++) {
    for (y=0; y<ROUNDWORLD_DIM.y; y++) { // Only half filled with water
      for (z=0; z<ROUNDWORLD_DIM.z; z++) {
        for (w=0; w<ROUNDWORLD_DIM.w; w++) {
          glm::dvec4 fromCenter = glm::dvec4(x, y, z, w) - (glm::dvec4(ROUNDWORLD_DIM) / 2.0);
          double rad = length(fromCenter);
          HyperCubeTypes h = hypercubes[x][y][z][w];
          if (rad < 9 && h == HCT_AIR) {
            hypercubes[x][y][z][w] = HCT_WATER;
          } else if (rad < 8 && (h == HCT_STONE || h == HCT_GRASS)) {
            if (rand() < RAND_MAX / 2) {
              hypercubes[x][y][z][w] = HCT_SAND;
            } else {
              hypercubes[x][y][z][w] = HCT_STONE;
            }
          }
        }
      }
    }
    std::cout << x << "/" << ROUNDWORLD_DIM.x << "\n";
  }
  /* for (x=0; x<ROUNDWORLD_DIM.x; x++) {
    for (y=0; y<ROUNDWORLD_DIM.y / 2; y++) { // Only half filled with water
      for (z=0; z<ROUNDWORLD_DIM.z; z++) {
        for (w=0; w<ROUNDWORLD_DIM.w; w++) {
          if (hypercubes[x][y][z][w] == HCT_AIR) {
            hypercubes[x][y][z][w] = HCT_WATER;
          }
        }
      }
    }
    std::cout << x << "/" << ROUNDWORLD_DIM.x << "\n";
    } */
  std::cout << "Done filling ponds" << std::endl;

  std::cout << "Starting mesh generation" << std::endl;

  for (x=0; x<ROUNDWORLD_DIM.x; x++) {
    for (y=0; y<ROUNDWORLD_DIM.y; y++) {
      for (z=0; z<ROUNDWORLD_DIM.z; z++) {
        for (w=0; w<ROUNDWORLD_DIM.w; w++) {
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
          case HCT_CLOUD:
            if (!SURROUNDED) {
              cloudLocs.push_back(glm::vec4(x, y, z, w));
            }
            break;
          default:
            std::cerr << "INVALID BLOCK TYPE " << hct << "\n";
            exit(-1);
          }
        }
      }
    }
    std::cout << x << "/" << ROUNDWORLD_DIM.x << "\n";
  }
  std::cout << "Done mesh generation" << std::endl;

}

void RoundWorld::draw() {
  std::cerr << "UNIMPLEMENTED\n";
  exit(-1);
}
