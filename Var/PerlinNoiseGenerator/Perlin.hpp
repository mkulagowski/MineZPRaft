#pragma once

#include <cmath>
#include <memory>
#include <fstream>


class NoiseGenerator
{
private:
	std::unique_ptr<int>(pPermutationTable);

	NoiseGenerator();
	NoiseGenerator(const NoiseGenerator&) = delete;
	NoiseGenerator& operator=(const NoiseGenerator&) = delete;
	~NoiseGenerator();

	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
	int perm(int index);

public:
	static NoiseGenerator& GetInstance();
	double Noise(double x, double y, double z);
};