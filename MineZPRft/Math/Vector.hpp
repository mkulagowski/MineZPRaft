/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Vector class declaration.
 */

#ifndef __MATH_VECTOR_HPP__
#define __MATH_VECTOR_HPP__

#include <iostream>

/**
 * Class used for vector operations.
 */
class Vector
{
public:

    /**
     * Access vector values
     * @return pointer to 4 float vector.
     */
    const float* Data() const;

    /**
     * Get vectors length
     */
    float Length() const;

    /**
     * Normalize vector
     */
    void Normalize();

    Vector();
    Vector(float a);
    Vector(float a, float b, float c, float d);
    ~Vector();

    Vector(const Vector& other);
    Vector(Vector&& other);
    Vector& operator= (const Vector& other);
    Vector& operator= (Vector&& other);

    // Access operator
    float operator[](int index) const;

    // Addition
    Vector& operator+=(const Vector& other);
    Vector& operator+=(float value);
    const Vector operator+(const Vector& other) const;
    const Vector operator+(float value) const;

    // Subtraction
    Vector& operator-=(const Vector& other);
    Vector& operator-=(float value);
    const Vector operator-(const Vector& other) const;
    const Vector operator-(float value) const;

    // Multiplication
    Vector& operator*=(const Vector& other);
    Vector& operator*=(float value);
    const Vector operator*(const Vector& other) const;
    const Vector operator*(float value) const;

    // Division
    Vector& operator/=(const Vector& other);
    Vector& operator/=(float value);
    const Vector operator/(const Vector& other) const;
    const Vector operator/(float value) const;

    // Power
    Vector& operator^(float value);

    // Products
    float Dot(const Vector& other);
    Vector Cross(const Vector& other) const;

    // Comparison
    bool operator==(const Vector& other) const;
    bool operator<(const Vector& other) const;
    bool operator>(const Vector& other) const;
    bool operator<=(const Vector& other) const;
    bool operator>=(const Vector& other) const;

    // Friendships
    friend std::ostream& operator<<(std::ostream& os, const Vector& v);

private:
    float f[4];
};

/**
 * Struct used for matrix-from-vector-creation
 */
struct Vector4
{
    Vector v[4];
};


/**
 * Overloaded operator << for easier printing and debugging.
 *
 * @param os STL output stream.
 * @param v Vector to print.
 *
 * @return STL output stream.
 */
std::ostream& operator<<(std::ostream& os, const Vector& v);

#endif // __MATH_VECTOR_HPP__
