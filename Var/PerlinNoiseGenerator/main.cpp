/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  PerlinNoiseGenerator Demo implementation.
 */

#include "Perlin.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    NoiseGenerator& noiseGen = NoiseGenerator::GetInstance();

    if (argc == 2)
    {
        long argumentSeed = strtol(argv[1], nullptr, 10);
        uint32_t uArgumentSeed = static_cast<uint32_t>(argumentSeed);
        noiseGen.CustomPermutationTable(uArgumentSeed);
        std::cout << argv[1] << ", " << argumentSeed << ", " << uArgumentSeed;
    }

    int width = 400;
    int height = 400;

    int arraySize = width * height * 3;
    uint8_t* imageArray = new uint8_t[arraySize];
    int index = 0;

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            double x_norm = static_cast<double>(x)
                / static_cast<double>(width);
            double y_norm = static_cast<double>(y)
                / static_cast<double>(height);

            double noiseVal = noiseGen.Noise(10 * x_norm, 10 * y_norm, 0.1);
            noiseVal = floor(noiseVal * 255);
            imageArray[index++] = static_cast<uint8_t>(noiseVal);
            imageArray[index++] = static_cast<uint8_t>(noiseVal);
            imageArray[index++] = static_cast<uint8_t>(noiseVal);
        }

    std::fstream rawFile;
    rawFile.open("perlin.raw", std::ios::out | std::ios::binary);
    rawFile.write(reinterpret_cast<char*>(imageArray), arraySize);
    rawFile.close();

    delete[] imageArray;

    return 0;
}