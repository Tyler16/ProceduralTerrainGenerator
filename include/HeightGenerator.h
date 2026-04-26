#ifndef HEIGHT_GENERATOR_H
#define HEIGHT_GENERATOR_H

#include "FastNoiseLite.h"

class HeightGenerator {
  public:
    HeightGenerator(int seed, float max_height);
    float getHeight(float world_x, float world_z);

  private:
    FastNoiseLite noiseGenerator_;
    int seed_;
    float max_height_;

    static constexpr float FREQUENCY = 0.05f;
};

#endif
