#include "more.h"

std::ostream &operator<<(std::ostream &os, glm::vec4 v) {
  return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}
