#include "HeightGenerator.h"
#include "FastNoiseLite.h"

HeightGenerator::HeightGenerator(int seed, float max_height) : 
    seed_(seed), max_height_(max_height) {
    noiseGenerator_.SetSeed(seed_);
    noiseGenerator_.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    noiseGenerator_.SetFractalType(FastNoiseLite::FractalType_FBm);
    noiseGenerator_.SetFractalOctaves(5);
    noiseGenerator_.SetFractalLacunarity(2.0f); 
    noiseGenerator_.SetFractalGain(0.5f);
}

float HeightGenerator::getHeight(float world_x, float world_z) {
    float noise_value = noiseGenerator_.GetNoise(world_x * FREQUENCY,
                                                world_z * FREQUENCY);
    float normalized_height = (noise_value + 1.0f) / 2.0f;
    return normalized_height * max_height_;
    //return noiseValue * max_height_;
}
