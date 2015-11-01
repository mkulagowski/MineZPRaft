#include "Perlin.hpp"

int main()
{
	NoiseGenerator& noiseGen = NoiseGenerator::GetInstance();

	int width = 400;
	int height = 400;

	int arraySize = width * height * 3;
	uint8_t* imageArray = new uint8_t[arraySize];
	int index = 0;

	for (double y = 0.0; y < height; y++)
		for (double x = 0.0; x < width; x++)
		{
			double x_norm = x / width;
			double y_norm = y / height;

			double noiseVal = noiseGen.Noise(10 * x_norm, 10 * y_norm, 0.8);
			noiseVal = floor(noiseVal * 255);
			imageArray[index] = static_cast<uint8_t>(noiseVal);
			imageArray[index + 1] = static_cast<uint8_t>(noiseVal);
			imageArray[index + 2] = static_cast<uint8_t>(noiseVal);
			index += 3;
		}

	std::fstream rawFile;
	rawFile.open("perlinRaw.raw", std::ios::out | std::ios::binary);
	rawFile.write(reinterpret_cast<char*>(imageArray), arraySize);
	rawFile.close();

	delete[] imageArray;

	return 0;
}