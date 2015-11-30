/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Vector class tests
 */

#include <gtest/gtest.h>
#include "Math/Vector.hpp"

TEST(Vector, AddVector)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector b(2.0f, 3.0f, 4.0f, 5.0f);

    ASSERT_EQ(Vector(3.0f, 5.0f, 7.0f, 9.0f), a + b);
}

TEST(Vector, AddFloat)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);

    ASSERT_EQ(Vector(3.0f, 4.0f, 5.0f, 6.0f), a + 2.0f);
}

TEST(Vector, SubtractVector)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector b(2.0f, 3.0f, 4.0f, 5.0f);

    ASSERT_EQ(Vector(-1.0f, -1.0f, -1.0f, -1.0f), a - b);
}

TEST(Vector, SubtractFloat)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);

    ASSERT_EQ(Vector(-1.0f, 0.0f, 1.0f, 2.0f), a - 2.0f);
}

TEST(Vector, MultiplyVector)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector b(2.0f, 3.0f, 4.0f, 5.0f);

    ASSERT_EQ(Vector(2.0f, 6.0f, 12.0f, 20.0f), a * b);
}

TEST(Vector, MultiplyFloat)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);

    ASSERT_EQ(Vector(2.0f, 4.0f, 6.0f, 8.0f), a * 2.0f);
}

TEST(Vector, DivideVector)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector b(2.0f, 4.0f, 6.0f, 8.0f);

    ASSERT_EQ(Vector(0.5f, 0.5f, 0.5f, 0.5f), a / b);
}

TEST(Vector, DivideFloat)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);

    ASSERT_EQ(Vector(0.5f, 1.0f, 1.5f, 2.0f), a / 2.0f);
}

TEST(Vector, Dot)
{
    Vector a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector b(2.0f, 3.0f, 4.0f, 5.0f);

    ASSERT_EQ(40.0f, a.Dot(b));
}

TEST(Vector, Cross)
{
    Vector a(1.0f, 0.0f, 0.0f, 0.0f);
    Vector b(0.0f, 1.0f, 0.0f, 0.0f);

    ASSERT_EQ(Vector(0.0f, 0.0f, 1.0f, 0.0f), a.Cross(b));
}
