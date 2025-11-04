#ifndef MAT3PULSE_H
#define MAT3PULSE_H

// #include "Common/common.h"
#include <cstring>
#include <cmath>

#include "PulseEngine/core/Math/Vector.h"

namespace PulseEngine
{

    
    inline float Dot(const Vector3& a, const Vector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    /**
     * @brief Cross product of two vectors.
     * 
     * @param a the first vector.
     * @param b the second vector.
     * @return Vector3 the cross product of a and b.
     */
    inline Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
    /**
     * @brief Normalize a vector.
     * 
     * @param v the vector to normalize.
     * @return Vector3 the normalized vector.
     */
    inline Vector3 Normalize(const Vector3& v)
    {
        float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (length > 1e-6f) // éviter la division par zéro
        {
            return Vector3(v.x / length, v.y / length, v.z / length);
        }
        return Vector3(0.0f, 0.0f, 0.0f); // vecteur nul si la norme est trop petite
    }
    /**
     * @brief Mat3 class represents a 3x3 matrix used for 3D transformations.
     * 
     */
    struct PULSE_ENGINE_DLL_API Mat3
    {
        float m[3][3]; // Ligne-colonne

        // Constructeurs
        Mat3()
        {
            std::memset(m, 0, sizeof(m));
        }

        Mat3(float diag)
        {
            std::memset(m, 0, sizeof(m));
            m[0][0] = m[1][1] = m[2][2] = diag;
        }

        Mat3(const float values[3][3])
        {
            std::memcpy(m, values, sizeof(m));
        }

        /**
         * @brief Access operator to get a row of the matrix.
         * 
         * @param row which row to access (0, 1, or 2).
         * @return float* return a pointer to the specified row of the matrix.
         */
        float* operator[](int row)
        {
            return m[row];
        }
        /**
         * @brief Access operator to get a row of the matrix.
         * 
         * @param row which row to access (0, 1, or 2).
         * @return const float* return a pointer to the specified row of the matrix.
         */
        const float* operator[](int row) const
        {
            return m[row];
        }

        /**
         * @brief Easily multiply a matrix by a vector.
         * 
         * @param v the actual vector to multiply with the matrix.
         * @return the result will be a vector3, which is the result of the multiplication of the matrix by the vector. 
         */
        Vector3 operator*(const Vector3& v) const
        {
            return Vector3(
                m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
                m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
                m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
            );
        }

        /**
         * @brief easily transpose a matrix.
         * 
         * @return Mat3 transposed matrix.
         */
        Mat3 Transpose() const
        {
            Mat3 result;
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    result.m[i][j] = m[j][i];
                }
            }
            return result;
        }
        /**
         * @brief Create an identity matrix (with 0.0f and 1.0f) effortlessly.
         * 
         * @return Mat3 an identity matrix.
         */
        static Mat3 Identity()
        {
            return Mat3(1.0f);
        }

        /**
         * @brief Create a rotation matrix around the X axis.
         * 
         * @param angleRadians the angle in radians to rotate around the X axis.
         * @return Mat3 the rotation matrix.
         */
        static Mat3 RotationX(float angleRadians)
        {
            float c = std::cos(angleRadians);
            float s = std::sin(angleRadians);

            Mat3 rot;
            rot.m[0][0] = 1.0f; rot.m[0][1] = 0.0f; rot.m[0][2] = 0.0f;
            rot.m[1][0] = 0.0f; rot.m[1][1] =  c;   rot.m[1][2] = -s;
            rot.m[2][0] = 0.0f; rot.m[2][1] =  s;   rot.m[2][2] =  c;

            return rot;
        }

        /**
         * @brief Create a rotation matrix around the Y axis.
         * 
         * @param angleRadians the angle in radians to rotate around the Y axis.
         * @return Mat3 the rotation matrix.
         */
        static Mat3 RotationY(float angleRadians)
        {
            float c = std::cos(angleRadians);
            float s = std::sin(angleRadians);

            Mat3 rot;
            rot.m[0][0] =  c;   rot.m[0][1] = 0.0f; rot.m[0][2] = s;
            rot.m[1][0] = 0.0f; rot.m[1][1] = 1.0f; rot.m[1][2] = 0.0f;
            rot.m[2][0] = -s;   rot.m[2][1] = 0.0f; rot.m[2][2] =  c;

            return rot;
        }

        /**
         * @brief Create a rotation matrix around the Z axis.
         * 
         * @param angleRadians the angle in radians to rotate around the Z axis.
         * @return Mat3 the rotation matrix.
         */
        static Mat3 RotationZ(float angleRadians)
        {
            float c = std::cos(angleRadians);
            float s = std::sin(angleRadians);

            Mat3 rot;
            rot.m[0][0] =  c;   rot.m[0][1] = -s;   rot.m[0][2] = 0.0f;
            rot.m[1][0] =  s;   rot.m[1][1] =  c;   rot.m[1][2] = 0.0f;
            rot.m[2][0] = 0.0f; rot.m[2][1] = 0.0f; rot.m[2][2] = 1.0f;

            return rot;
        }

        /**
         * @brief Create a rotation matrix from Euler angles (in radians).
         * 
         * @param rx rotation around the X axis.
         * @param ry rotation around the Y axis.
         * @param rz rotation around the Z axis.
         * @return Mat3 the rotation matrix.
         */
        static Mat3 FromEulerXYZ(float rx, float ry, float rz)
        {
            return RotationZ(rz) * RotationY(ry) * RotationX(rx);
        }

        /**
         * @brief Multiply this matrix by another matrix.
         * 
         * @param other the other matrix to multiply with.
         * @return Mat3 the resulting matrix after multiplication.
         */
        Mat3 operator*(const Mat3& other) const
        {
            Mat3 result;
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    result.m[i][j] = m[i][0] * other.m[0][j] +
                                     m[i][1] * other.m[1][j] +
                                     m[i][2] * other.m[2][j];
                }
            }
            return result;
        }
    };
}

#endif // MAT3PULSE_H
