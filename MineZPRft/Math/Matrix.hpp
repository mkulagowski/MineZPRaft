/**
 * @file
 * @author mkkulagowski (mkkulagowski(at)gmail.com)
 * @brief  Matrix class declaration.
 */

#ifndef __MATH_MATRIX_HPP__
#define __MATH_MATRIX_HPP__

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
    float operator[](int index) const;

    // Addition
    Matrix& operator+=(const Matrix& other);
    Matrix& operator+=(float value);
    const Matrix operator+(const Matrix& other) const;
    const Matrix operator+(float value) const;

    // Subtraction
    Matrix& operator-=(const Matrix& other);
    Matrix& operator-=(float value);
    const Matrix operator-(const Matrix& other) const;
    const Matrix operator-(float value) const;

    // Multiplication
    Matrix& operator*=(const Matrix& other);
    Matrix& operator*=(float value);
    const Matrix operator*(const Matrix& other) const;
    const Matrix operator*(float value) const;

    // Division
    Matrix& operator/=(float value);
    const Matrix operator/(float value) const;

    // Transposition
    Matrix& operator~();

    // Power
    Matrix& operator^(float value);

    // Comparison
    bool operator==(const Matrix& other) const;
    bool operator<(const Matrix& other) const;
    bool operator>(const Matrix& other) const;
    bool operator<=(const Matrix& other) const;
    bool operator>=(const Matrix& other) const;

    // Friendship
    friend Vector operator*(const Matrix& a, const Vector& b);

private:
    float f[4 * 4];
};


// External operators (cross-type)
Vector operator*(const Matrix& a, const Vector& b);


/**
 * Creates right-handed Look At Matrix (aka. View Matrix)
 *
 * @param pos Position of camera
 * @param dir Direction of camera (aka. Look At vector)
 * @param up  Up Vector of the camera
 *
 * @return Calculated right-handed Look At Matrix
 */
Matrix CreateRHLookAtMatrix(const Vector& pos, const Vector& dir, const Vector& up);

/**
 * Creates a right-handed Perspective Matrix (aka. Projection Matrix)
 *
 * @param fov         Field of View in degrees
 * @param aspectRatio Screen width to screen height ratio
 * @param nearDist    Near distance of the Perspective. Any objects closer than this distance
 *                    will clip.
 * @param farDist     Far distance of the Perspective. Any objects further than this distance
 *                    will clip.
 *
 * @return Right-handed perspective matrix.
 */
Matrix CreateRHPerspectiveMatrix(const float fov, const float aspectRatio,
                                 const float nearDist, const float farDist);

/**
 * Creates a rotation matrix, which multiplied by a Vector will rotate it by @p angle radians along
 * X axis.
 *
 * @param angle Angle in radians with which rotation is applied
 *
 * @return Rotation matrix along X axis
 */
Matrix CreateRotationMatrixX(const float angle);

/**
 * Creates a rotation matrix, which multiplied by a Vector will rotate it by @p angle radians along
 * Y axis.
 *
 * @param angle Angle in radians with which rotation is applied
 *
 * @return Rotation matrix along Y axis
 */
Matrix CreateRotationMatrixY(const float angle);

/**
 * Creates a rotation matrix, which multiplied by a Vector will rotate it by @p angle radians along
 * Z axis.
 *
 * @param angle Angle in radians with which rotation is applied
 *
 * @return Rotation matrix along Z axis
 */
Matrix CreateRotationMatrixZ(const float angle);


#endif // __MATH_MATRIX_HPP__
