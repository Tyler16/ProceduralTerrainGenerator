#ifndef HEIGHT_GENERATOR_H
#define HEIGHT_GENERATOR_H

#include <FastNoise/FastNoise.h>

class HeightGenerator {
  public:
    HeightGenerator(int seed, float max_height);
    void getHeightMap(float* buffer, int chunk_x, int chunk_z, int size);

  private:
    FastNoise::SmartNode<> noise_generator_;
    int seed_;
    float max_height_;

    static constexpr float FREQUENCY = 0.05f;
};

#endif
