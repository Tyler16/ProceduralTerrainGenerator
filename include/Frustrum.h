#ifndef FRUSTRUM_H
#define FRUSTRUM_H

#include "Plane.h"

#include <glm/glm.hpp>

class Frustrum {
  public:
    Frustrum(const glm::mat4& m);
    bool isInside(const glm::vec3& center, const glm::vec3& extents);

  private:
    static constexpr int NUM_PLANES = 6;
    Plane planes_[NUM_PLANES];
};

#endif
