/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  NoiseGenerator class implementation.
 */

#include "NoiseGenerator.hpp"

NoiseGenerator::NoiseGenerator()
{
    // Initialize default permutation table
    mPermutationTable = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96,
                        53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8,
                        99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120,
                        234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117,
                        35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174,
                        20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134,
                        139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229,
                        122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46,
                        245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1,
                        216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169,
                        200, 196, 135, 130, 116, 188, 159, 86, 164, 100,
                        109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124,
                        123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212,
                        207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28,
                        42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154,
                        163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129,
                        22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
                        178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
                        242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
                        81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214,
                        31, 181, 199, 106, 157, 184, 84, 204, 176, 115,
                        121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93,
                        222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78,
                        66, 215, 61, 156, 180};

    // Fill out the rest of the table
    auto begin = mPermutationTable.begin();
    auto end = mPermutationTable.end();
    mPermutationTable.insert(end, begin, end);
}

NoiseGenerator::~NoiseGenerator()
{
}

NoiseGenerator& NoiseGenerator::GetInstance()
{
    static NoiseGenerator instance;
    return instance;
}

// Cross-fade ( S curve ) function
double NoiseGenerator::Fade(double t) const
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// TODO consider moving to Math library
// Linear interpolation function
double NoiseGenerator::Lerp(double t, double a, double b) const
{
    return a + t * (b - a);
}

// Function that returns uniformly random values of x,y,z via given hash value
double NoiseGenerator::Grad(int hash, double x, double y, double z) const
{
    // Get lower 4 bits of hash code instead of making modulos
    int h = hash & 15;

    // Convert them into 12 gradient directions
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;

    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Helper function to make code readable
int NoiseGenerator::Perm(int index) const
{
    return mPermutationTable[index];
}

void NoiseGenerator::CustomPermutationTable(uint32_t seed)
{
    // Initialize custom permutation table
    mPermutationTable.clear();
    mPermutationTable.resize(256);
    auto begin = mPermutationTable.begin();
    auto end = mPermutationTable.end();

    // Produce values 0 - 255
    std::iota(begin, end, 0);

    // Shuffle them
    std::default_random_engine engine(seed);
    std::shuffle(begin, end, engine);

    // Fill out the rest of the table
    mPermutationTable.insert(end, begin, end);
}

double NoiseGenerator::Noise(double x, double y, double z) const
{
    // Find 8point cube that contains the given point
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    int Z = static_cast<int>(floor(z)) & 255;

    // Find relative X, Y, Z of the point in this cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute cross-fade curves in each of x, y and z
    double u = Fade(x);
    double v = Fade(y);
    double w = Fade(z);

    // Hash coordinates of the 8 cube corners
    int A = Perm(X) + Y;
    int AA = Perm(A) + Z;
    int AB = Perm(A + 1) + Z;
    int B = Perm(X + 1) + Y;
    int BA = Perm(B) + Z;
    int BB = Perm(B + 1) + Z;

    // Time to interpolate results from the 8 corners of the cube
    //     Possible interpolations in X axis (4 of them)
    double x11 = Lerp(u, Grad(Perm(AA), x, y, z),
                      Grad(Perm(BA), x - 1, y, z));

    double x12 = Lerp(u, Grad(Perm(AB), x, y - 1, z),
                      Grad(Perm(BB), x - 1, y - 1, z));

    double x21 = Lerp(u, Grad(Perm(AA + 1), x, y, z - 1),
                      Grad(Perm(BA + 1), x - 1, y, z - 1));

    double x22 = Lerp(u, Grad(Perm(AB + 1), x, y - 1, z - 1),
                      Grad(Perm(BB + 1), x - 1, y - 1, z - 1));

    //     Possible interpolations in Y axis (2 of them)
    double y1 = Lerp(v, x11, x12);
    double y2 = Lerp(v, x21, x22);

    //     Possible interpolations in Z axis (1 of them)
    double z1 = Lerp(w, y1, y2);

    // Return blended result from the 8 corners of the cube
    return z1;
}