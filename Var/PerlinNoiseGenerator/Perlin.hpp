/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  PerlinNoiseGenerator class declaration.
 */

#pragma once

#include <cmath>
#include <memory>
#include <fstream>
#include <numeric>
#include <random>
#include <algorithm>

/**
 * Class used for 3D Perlin noise generation.
 */
class NoiseGenerator
{
private:
    std::vector<int> mPermutationTable;

    NoiseGenerator();
    NoiseGenerator(const NoiseGenerator&) = delete;
    NoiseGenerator& operator=(const NoiseGenerator&) = delete;
    ~NoiseGenerator();

    double Fade(double t) const;
    double Lerp(double t, double a, double b) const;
    double Grad(int hash, double x, double y, double z) const;
    int Perm(int index) const;

public:

    /**
     * Get NoiseGenerator instance
     */
    static NoiseGenerator& GetInstance();

    /**
     * Recreate permutation table using given seed.
     * @param  seed custom seed
     */
    void CustomPermutationTable(uint32_t seed);

    /**
     * Generate Perlin noise for given point in unit cube
     * @param  x position on x axis
     * @param  y position on y axis
     * @param  z position on z axis
     * @return generated random value
     */
    double Noise(double x, double y, double z) const;
};