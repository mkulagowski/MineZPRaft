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
