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

NoiseGenerator::NoiseGenerator()
{
    // Initialize permutation table
    pPermutationTable.reset(new int[512]{151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    });

    // Fill out the rest of the table
    for (int i = 0; i < 256; i++)
        pPermutationTable.get()[256 + i] = pPermutationTable.get()[i];
}

NoiseGenerator::~NoiseGenerator()
{
}

NoiseGenerator& NoiseGenerator::GetInstance()
{
    static NoiseGenerator instance;
    return instance;
}

double NoiseGenerator::Noise(double x, double y, double z)
{
    // Find 8point cube that contains the given point
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    // Find relative X, Y, Z of the point in this cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute cross-fade curves in each of x, y and z
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = perm(X) + Y;
    int AA = perm(A) + Z;
    int AB = perm(A + 1) + Z;
    int B = perm(X + 1) + Y;
    int BA = perm(B) + Z;
    int BB = perm(B + 1) + Z;

    // Time to interpolate results from the 8 corners of the cube
    //     Possible interpolations in X axis (4 of them)
    double x11 = lerp(u, grad(perm(AA), x, y, z), grad(perm(BA), x - 1, y, z));
    double x12 = lerp(u, grad(perm(AB), x, y - 1, z), grad(perm(BB), x - 1, y - 1, z));
    double x21 = lerp(u, grad(perm(AA + 1), x, y, z - 1), grad(perm(BA + 1), x - 1, y, z - 1));
    double x22 = lerp(u, grad(perm(AB + 1), x, y - 1, z - 1), grad(perm(BB + 1), x - 1, y - 1, z - 1));

    //     Possible interpolations in Y axis (2 of them)
    double y1 = lerp(v, x11, x12);
    double y2 = lerp(v, x21, x22);

    //     Possible interpolations in Z axis (1 of them)
    double z1 = lerp(w, y1, y2);

    // Return blended result from the 8 corners of the cube
    return z1;
}

// Helper function to make code readable
int NoiseGenerator::perm(int index)
{
    return pPermutationTable.get()[index];
}

// Cross-fade ( S curve ) function
double NoiseGenerator::fade(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// TODO consider moving to common library
// Linear interpolation function
double NoiseGenerator::lerp(double t, double a, double b)
{
    return a + t * (b - a);
}

// Function that returns uniformly random values of x,y and z via given hash value
double NoiseGenerator::grad(int hash, double x, double y, double z)
{
    // Get lower 4 bits of hash code instead of making modulos
    int h = hash & 15;

    // Convert them into 12 gradient directions
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;

    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

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