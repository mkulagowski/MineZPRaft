/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Vector class definitions.
 */

#include "Vector.hpp"

#include <algorithm>
#include <cstring>

Vector::Vector()
    : Vector(0.0f)
{
}

Vector::Vector(float a)
{
    for (int i = 0; i < 4; ++i)
        f[i] = a;
}

Vector::Vector(float a, float b, float c, float d)
    : f{a,b,c,d}
{
}

Vector::~Vector()
{
}

Vector::Vector(const Vector& other)
{
    memcpy(f, other.f, sizeof(float) * 4);
}

Vector::Vector(Vector&& other)
    : Vector()
{
    std::swap(f, other.f);
}

const float* Vector::Data() const
{
    return f;
}

float Vector::Length() const
{
    Vector temp(*this);

    return sqrtf(temp.Dot(temp));
}

void Vector::Normalize()
{
    float len = Length();

    if (len)
        *this = *this / len;
}

Vector& Vector::operator= (const Vector& other)
{
    Vector tmp(other);
    *this = std::move(tmp);

    return *this;
}

Vector& Vector::operator= (Vector&& other)
{
    std::swap(f, other.f);

    return *this;
}

// Access operator
float Vector::operator[](int index) const
{
    return f[index];
}

// Addition
Vector& Vector::operator+=(const Vector& other)
{
    for (int i = 0; i < 4; ++i)
        f[i] += other.f[i];

    return *this;
}

Vector& Vector::operator+=(float value)
{
    for (auto& i : f)
        i += value;

    return *this;
}

const Vector Vector::operator+(const Vector& other) const
{
    return Vector(*this) += other;
}

const Vector Vector::operator+(float value) const
{
    return Vector(*this) += value;
}

// Subtraction
Vector& Vector::operator-=(const Vector& other)
{
    for (int i = 0; i < 4; ++i)
        f[i] -= other.f[i];

    return *this;
}

Vector& Vector::operator-=(float value)
{
    for (auto& i : f)
        i -= value;

    return *this;
}

const Vector Vector::operator-(const Vector& other) const
{
    return Vector(*this) -= other;
}

const Vector Vector::operator-(float value) const
{
    return Vector(*this) -= value;
}

// Multiplication
Vector& Vector::operator*=(const Vector& other)
{
    for (int i = 0; i < 4; ++i)
        f[i] *= other.f[i];

    return *this;
}

Vector& Vector::operator*=(float value)
{
    for (auto& i : f)
        i *= value;

    return *this;
}

const Vector Vector::operator*(const Vector& other) const
{
    return Vector(*this) *= other;
}

const Vector Vector::operator*(float value) const
{
    return Vector(*this) *= value;
}

// Division
Vector& Vector::operator/=(const Vector& other)
{
    for (int i = 0; i < 4; ++i)
        f[i] /= other.f[i];

    return *this;
}

Vector& Vector::operator/=(float value)
{
    for (auto& i : f)
        i /= value;

    return *this;
}

const Vector Vector::operator/(const Vector& other) const
{
    return Vector(*this) /= other;
}

const Vector Vector::operator/(float value) const
{
    return Vector(*this) /= value;
}

// Power
Vector& Vector::operator^(float value)
{
    for (auto& i : f)
        i = pow(i, value);

    return *this;
}

// Products
float Vector::Dot(const Vector& other)
{
    Vector tmp(*this);
    tmp = tmp * other;

    float result = 0;
    for (auto i : tmp.f)
        result += i;

    return result;
}

Vector Vector::Cross(const Vector& other) const
{
    Vector result(*this);

    // assume this is a 3D vector and calculate cross according to xyzzy mnemonic
    result.f[0] = this->f[1] * other.f[2] - this->f[2] * other.f[1];
    result.f[1] = this->f[2] * other.f[0] - this->f[0] * other.f[2];
    result.f[2] = this->f[0] * other.f[1] - this->f[1] * other.f[0];
    // result.f[3] remains unchanged

    return result;
}

// Comparison
bool Vector::operator==(const Vector& other) const
{
    for (int i = 0; i < 4; ++i)
        if (f[i] != other.f[i])
            return false;

    return true;
}

bool Vector::operator<(const Vector& other) const
{
    for (int i = 0; i < 4; ++i)
        if (f[i] >= other.f[i])
            return false;

    return true;
}

bool Vector::operator>(const Vector& other) const
{
    for (int i = 0; i < 4; ++i)
        if (f[i] <= other.f[i])
            return false;

    return true;
}

bool Vector::operator<=(const Vector& other) const
{
    for (int i = 0; i < 4; ++i)
        if (f[i] > other.f[i])
            return false;

    return true;
}

bool Vector::operator>=(const Vector& other) const
{
    for (int i = 0; i < 4; ++i)
        if (f[i] < other.f[i])
            return false;

    return true;
}

// Friendships
std::ostream& operator<<(std::ostream& os, const Vector& v)
{
    os << "[" << v.f[0] << ", " << v.f[1] << ", " << v.f[2] << ", " << v.f[3] << "]";
    return os;
}
