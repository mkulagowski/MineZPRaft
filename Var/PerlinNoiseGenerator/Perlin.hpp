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

class NoiseGenerator
{
private:
    std::vector<int> mPermutationTable;

    NoiseGenerator();
    NoiseGenerator(uint32_t seed);
	NoiseGenerator(const NoiseGenerator&) = delete;
	NoiseGenerator& operator=(const NoiseGenerator&) = delete;
	~NoiseGenerator();

	double Fade(double t) const;
	double Lerp(double t, double a, double b) const;
	double Grad(int hash, double x, double y, double z) const;
	int Perm(int index) const;

public:
	static NoiseGenerator& GetInstance();
	double Noise(double x, double y, double z) const;
};