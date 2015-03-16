#include "world.h"
#include "noise.h"

static const size_t EST_VERT_COUNT = 495360;

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

  size_t vertCount = 0;
  size_t elemCount = 0;

  // Create the hypercubes array
  // HyperCubeTypes hypercubes[WORLD_DIM.x][WORLD_DIM.y][WORLD_DIM.z][WORLD_DIM.w];

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

          if (sample < 1) {
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

  // Reserve a pre-computed amount of space for these verts
  // TODO(michael): Maybe don't make this manual
  TesseractVert tmpVerts[Tesseract::OUT_SIZE];
  verts.reserve(EST_VERT_COUNT);

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
              Tesseract::withOffset(glm::vec4(x, y, z, w), tmpVerts);
              for (size_t i=0; i<Tesseract::OUT_SIZE; i++) {
                verts.push_back(tmpVerts[i]);
              }

              // Actually add the verts and elements
              vertCount += 16;
              elemCount += 24*4;
            }
            // TODO(michael): Check if the adjacent cubes are filled
            break;
          }
        }
      }
    }
    std::cout << x << "/" << WORLD_DIM.x << "\n";
  }
  std::cout << "Done mesh generation" << std::endl;

  std::cout << "vertCount = " << vertCount << "\n";
  std::cout << "elemCount = " << elemCount << "\n";
  std::cout << "verts.size() = " << verts.size() << "\n";

  std::cout << "Creating VAO/VBO" << std::endl;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Create & Bind the VBO
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Load the data
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(verts),
               verts.data(),
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  std::cout << "Done Creating VAO/VBO" << std::endl;

}

void World::draw() {
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]));
}
