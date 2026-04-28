#include "HeightGenerator.h"

#include "GlobalConstants.h"

#include <FastNoise/FastNoise.h>

HeightGenerator::HeightGenerator(int seed, float max_height) : 
    seed_(seed), max_height_(max_height) {

    auto simplex = FastNoise::New<FastNoise::Simplex>();
    
    auto fbm = FastNoise::New<FastNoise::FractalFBm>();
    fbm->SetSource(simplex);
    fbm->SetOctaveCount(5);
    fbm->SetLacunarity(2.0f);
    fbm->SetGain(0.5f);

    noise_generator_ = fbm;
}

void HeightGenerator::getHeightMap(float* buffer, int chunk_x, int chunk_z, int size) {
    float start_x = (static_cast<float>(chunk_x) * FREQUENCY * static_cast<float>(Constants::Chunks::RESOLUTION) - 1.0f);
    float start_z = (static_cast<float>(chunk_z) * FREQUENCY * static_cast<float>(Constants::Chunks::RESOLUTION) - 1.0f);

    noise_generator_->GenUniformGrid2D(buffer,
                                       start_x, start_z,
                                       size, size,
                                       FREQUENCY, FREQUENCY, seed_);

    for (int i = 0; i < size * size; ++i) {
        float normalized = (buffer[i] + 1.0f) * 0.5f;
        buffer[i] = normalized * max_height_;
    }
}
