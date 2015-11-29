/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Matrix class implementation.
 */

#include "Matrix.hpp"

#include <algorithm>

Matrix::Matrix()
    : f{{0}}
{
};

Matrix::Matrix(float a)
    : f{{a}}
{
};

Matrix::Matrix(float a1, float a2, float a3, float a4,
               float b1, float b2, float b3, float b4,
               float c1, float c2, float c3, float c4,
               float d1, float d2, float d3, float d4)
    : f{a1,a2,a3,a4, b1,b2,b3,b4, c1,c2,c3,c4, d1,d2,d3,d4}
{
};

Matrix::Matrix(Vector& a, Vector& b, Vector& c, Vector& d)
{
    for (int i = 0; i < 4; i++)
    {
        f[i] = a[i];
        f[i + 4 * 1] = b[i];
        f[i + 4 * 2] = c[i];
        f[i + 4 * 3] = d[i];
    }
};

Matrix::Matrix(Vector4* a)
    : Matrix(a->v[0], a->v[1], a->v[2], a->v[3])
{
};

Matrix::~Matrix()
{
};

Matrix::Matrix(const Matrix& other)
{
    memcpy(f, other.f, sizeof(float) * 16);
};

Matrix::Matrix(Matrix&& other)
    : Matrix()
{
    std::swap(f, other.f);
};

const float* Matrix::Data()
{
    return f;
}

Matrix& Matrix::operator= (const Matrix& other)
{
    Matrix tmp(other);
    *this = std::move(tmp);

    return *this;
};

Matrix& Matrix::operator= (Matrix&& other)
{
    std::swap(f, other.f);

    return *this;
};

const float Matrix::operator[](int index)
{
    return f[index];
}

// Addition
Matrix& Matrix::operator+(const Matrix& other)
{
    for (int i = 0; i < 4 * 4; i++)
        f[i] += other.f[i];

    return *this;
};

Matrix& Matrix::operator+(float value)
{
    for (auto i : f)
        i += value;

    return *this;
};

// Subtraction
Matrix& Matrix::operator-(const Matrix& other)
{
    for (int i = 0; i < 4 * 4; i++)
        f[i] -= other.f[i];

    return *this;
};

Matrix& Matrix::operator-(float value)
{
    for (auto i : f)
        i -= value;

    return *this;
};

// Multiplication
Matrix& Matrix::operator*(const Matrix& other)
{
    Matrix temp;
    float result;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            result = 0;
            for (int k = 0; k < 4; k++)
                result += f[4 * i + k] * other.f[4 * k + j];
            temp.f[4 * i + j] = result;
        }

    std::swap(f, temp.f);

    return *this;
};

Matrix& Matrix::operator*(float value)
{
    for (auto i : f)
        i *= value;

    return *this;
};

// Division
Matrix& Matrix::operator/(float value)
{
    for (auto i : f)
        i /= value;

    return *this;
};

// Transposition
Matrix& Matrix::operator~()
{
    float tmp[16] = {{0}};

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            tmp[i * 4 + j] = f[i + 4 * j];

    std::swap(f, tmp);

    return *this;
};

// Power
Matrix& Matrix::operator^(float value)
{
    for (auto i : f)
        i = pow(i, value);

    return *this;
};
