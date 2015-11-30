/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Matrix class tests
 */

#include <gtest/gtest.h>
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

/*
 * There are no tests for CreateRHLookAtMatrix and CreateRHPerspectiveMatrix,
 * Both of these functions are a specific solutions developed for graphics use. The functions
 * will be tested after Camera module implementation.
 */

TEST(Matrix, Constructor)
{
    Matrix a;

    ASSERT_EQ(Matrix(0.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 0.0f),
              a);
}

TEST(Matrix, ConstructorSingleFloat)
{
    Matrix a(1.0f);

    ASSERT_EQ(Matrix(1.0f, 1.0f, 1.0f, 1.0f,
                     1.0f, 1.0f, 1.0f, 1.0f,
                     1.0f, 1.0f, 1.0f, 1.0f,
                     1.0f, 1.0f, 1.0f, 1.0f),
              a);
}

TEST(Matrix, ConstructorFromVectors)
{
    Vector a(1.0f);
    Vector b(2.0f);
    Vector c(3.0f);
    Vector d(4.0f);

    Matrix m(a, b, c, d);

    ASSERT_EQ(Matrix(1.0f, 1.0f, 1.0f, 1.0f,
                     2.0f, 2.0f, 2.0f, 2.0f,
                     3.0f, 3.0f, 3.0f, 3.0f,
                     4.0f, 4.0f, 4.0f, 4.0f),
              m);
}

TEST(Matrix, ConstructorFromVector4)
{
    Vector4 vec;
    vec.v[0] = Vector(1.0f);
    vec.v[1] = Vector(2.0f);
    vec.v[2] = Vector(3.0f);
    vec.v[3] = Vector(4.0f);

    Matrix m(&vec);

    ASSERT_EQ(Matrix(1.0f, 1.0f, 1.0f, 1.0f,
                     2.0f, 2.0f, 2.0f, 2.0f,
                     3.0f, 3.0f, 3.0f, 3.0f,
                     4.0f, 4.0f, 4.0f, 4.0f),
              m);
}

TEST(Matrix, AddMatrix)
{
    Matrix a(1.0f);
    Matrix b(2.0f);

    ASSERT_EQ(Matrix(3.0f), a + b);
}

TEST(Matrix, AddFloat)
{
    Matrix a(1.0f);

    ASSERT_EQ(Matrix(3.0f), a + 2.0f);
}

TEST(Matrix, SubtractMatrix)
{
    Matrix a(1.0f);
    Matrix b(2.0f);

    ASSERT_EQ(Matrix(-1.0f), a - b);
}

TEST(Matrix, SubtractFloat)
{
    Matrix a(1.0f);

    ASSERT_EQ(Matrix(-1.0f), a - 2.0f);
}

TEST(Matrix, MultiplyMatrix)
{
    Matrix a(1.0f);
    Matrix b(2.0f);

    ASSERT_EQ(Matrix(8.0f), a * b);
}

TEST(Matrix, MultiplyVector)
{
    Matrix a(2.0f);
    Vector b(1.0f, 2.0f, 3.0f, 4.0f);

    ASSERT_EQ(Vector(20.0f), a * b);
}

TEST(Matrix, MultiplyFloat)
{
    Matrix a(1.0f);

    ASSERT_EQ(Matrix(2.0f), a * 2.0f);
}

TEST(Matrix, DivideFloat)
{
    Matrix a(1.0f);

    ASSERT_EQ(Matrix(0.5f), a / 2.0f);
}

TEST(Matrix, Power)
{
    Matrix a(2.0f);

    ASSERT_EQ(Matrix(8.0f), a ^ 3.0f);
}

TEST(Matrix, Transpose)
{
    Matrix a( 1.0f,  2.0f,  3.0f,  4.0f,
              5.0f,  6.0f,  7.0f,  8.0f,
              9.0f, 10.0f, 11.0f, 12.0f,
             13.0f, 14.0f, 15.0f, 16.0f);

    Matrix t( 1.0f, 5.0f,  9.0f, 13.0f,
              2.0f, 6.0f, 10.0f, 14.0f,
              3.0f, 7.0f, 11.0f, 15.0f,
              4.0f, 8.0f, 12.0f, 16.0f);

    ASSERT_EQ(t, ~a);
}

TEST(Matrix, Equal)
{
    Matrix a(1.0f);

    ASSERT_EQ(a, a);
}

TEST(Matrix, Greater)
{
    Matrix a(1.0f);
    Matrix b(2.0f);

    ASSERT_GT(b, a);
}

TEST(Matrix, GreaterEqual)
{
    Matrix a(1.0f);
    Matrix b(2.0f);
    Matrix c(1.0f);

    ASSERT_GE(b, a);
    ASSERT_GE(c, a);
}

TEST(Matrix, Less)
{
    Matrix a(1.0f);
    Matrix b(2.0f);

    ASSERT_LT(a, b);
}

TEST(Matrix, LessEqual)
{
    Matrix a(1.0f);
    Matrix b(2.0f);
    Matrix c(1.0f);

    ASSERT_LE(a, b);
    ASSERT_LE(a, c);
}
