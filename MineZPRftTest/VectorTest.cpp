/**
 * @file
 * @author LKostyra (costyrra.xl@gmail.com)
 * @brief  Vector class tests
 */

#include <gtest/gtest.h>
#include "Math/Vector.hpp"


TEST(Vector, Constructor)
{
    Vector a;

    ASSERT_EQ(Vector(0.0f, 0.0f, 0.0f, 0.0f), a);
}

TEST(Vector, ConstructorSingleFloat)
{
    Vector a(1.0f);

    ASSERT_EQ(Vector(1.0f, 1.0f, 1.0f, 1.0f), a);
}

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

TEST(Vector, Power)
{
    Vector a(2.0f);

    ASSERT_EQ(Vector(8.0f), a ^ 3.0f);
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

TEST(Vector, Length)
{
    Vector a(2.0f, 0.0f, 0.0f, 0.0f);

    ASSERT_EQ(2.0f, a.Length());
}

TEST(Vector, Normalize)
{
    Vector a(2.0f, 0.0f, 0.0f, 0.0f);
    a.Normalize();

    ASSERT_EQ(Vector(1.0f, 0.0f, 0.0f, 0.0f), a);
}

TEST(Vector, Equal)
{
    Vector a(1.0f);

    ASSERT_EQ(a, a);
}

TEST(Vector, Greater)
{
    Vector a(1.0f);
    Vector b(2.0f);

    ASSERT_GT(b, a);
}

TEST(Vector, GreaterEqual)
{
    Vector a(1.0f);
    Vector b(2.0f);
    Vector c(1.0f);

    ASSERT_GE(b, a);
    ASSERT_GE(c, a);
}

TEST(Vector, Less)
{
    Vector a(1.0f);
    Vector b(2.0f);

    ASSERT_LT(a, b);
}

TEST(Vector, LessEqual)
{
    Vector a(1.0f);
    Vector b(2.0f);
    Vector c(1.0f);

    ASSERT_LE(a, b);
    ASSERT_LE(a, c);
}
