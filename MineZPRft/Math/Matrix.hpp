/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Matrix class declaration.
 */

#pragma once

#include "Vector.hpp"

/**
 * Class used for matrix operations.
 */
class Matrix
{
public:

    /**
     * Access matrix values
     * @return pointer to 4x4 float matrix.
     */
    const float* Data();

    Matrix();
    Matrix(float a);
    Matrix(float a1, float a2, float a3, float a4,
           float b1, float b2, float b3, float b4,
           float c1, float c2, float c3, float c4,
           float d1, float d2, float d3, float d4);
    Matrix(Vector& a, Vector& b, Vector& c, Vector& d);
    Matrix(Vector4* a);
    ~Matrix();

    Matrix(const Matrix& other);
    Matrix(Matrix&& other);
    Matrix& operator= (const Matrix& other);
    Matrix& operator= (Matrix&& other);

    // Access operators
    const float operator[](int index);

    // Addition
    Matrix& operator+(const Matrix& other);
    Matrix& operator+(float value);

    // Subtraction
    Matrix& operator-(const Matrix& other);
    Matrix& operator-(float value);

    // Multiplication
    Matrix& operator*(const Matrix& other);
    Matrix& operator*(float value);

    // Division
    Matrix& operator/(float value);

    // Transposition
    Matrix& operator~();

    // Power
    Matrix& operator^(float value);

private:
    float f[4 * 4];
};