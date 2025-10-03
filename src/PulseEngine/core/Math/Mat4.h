#ifndef MAT4PULSE_H
#define MAT4PULSE_H

#include <cstring> 
#include "PulseEngine/core/Math/Vector.h"

namespace PulseEngine
{
    /**
     * @brief Mat4 class represents a 4x4 matrix used for 3D transformations.
     * 
     */
    struct PULSE_ENGINE_DLL_API Mat4
    {
        float data[4][4]; 

        // Default constructor initializes to identity matrix
        Mat4()
        {
            SetIdentity();
        }

        // Constructor that sets the diagonal to a scalar value
        Mat4(float scalar)
        {
            memset(data, 0, sizeof(data));
            for (int i = 0; i < 4; ++i)
            {
                data[i][i] = scalar;
            }
        }

        Mat4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33)
        {
            data[0][0] = m00; data[0][1] = m01; data[0][2] = m02; data[0][3] = m03;
            data[1][0] = m10; data[1][1] = m11; data[1][2] = m12; data[1][3] = m13;
            data[2][0] = m20; data[2][1] = m21; data[2][2] = m22; data[2][3] = m23;
            data[3][0] = m30; data[3][1] = m31; data[3][2] = m32; data[3][3] = m33;
        }

        
        /**
         * @brief Set the Identity object
         * 
         */
        void SetIdentity()
        {
            memset(data, 0, sizeof(data));
            for (int i = 0; i < 4; ++i)
            {
                data[i][i] = 1.0f;
            }
        }

        /**
         * @brief Access operator to get a row of the matrix.
         * 
         * @param row which row to access (0, 1, 2, or 3).
         * @return float* return a pointer to the specified row of the matrix.
         */
        float* operator[](int row)
        {
            return data[row];
        }

        /**
         * @brief Access operator to get a row of the matrix.
         * 
         * @param row which row to access (0, 1, 2, or 3).
         * @return const float* return a pointer to the specified row of the matrix.
         */
        const float* operator[](int row) const
        {
            return data[row];
        }

        /**
         * @brief Get a pointer to the underlying data.
         * 
         * @return const float* pointer to the first element of the matrix.
         */
        const float* Ptr() const
        {
            return &data[0][0];
        }

        /**
         * @brief Get a pointer to the underlying data.
         * 
         * @return float* pointer to the first element of the matrix.
         */
        float* Ptr()
        {
            return &data[0][0];
        }

        /**
         * @brief Multiply this matrix by another matrix.
         * 
         * @param other the other matrix to multiply with.
         * @return Mat4 the resulting matrix after multiplication.
         */
        Mat4 operator*(const Mat4& other) const
        {
            Mat4 result(0.0f);
            for (int col = 0; col < 4; ++col)
            {
                for (int row = 0; row < 4; ++row)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        result.data[col][row] += data[i][row] * other.data[col][i];
                    }
                }
            }
            return result;
        }


        /**
         * @brief Multiply this matrix by a vector.
         * 
         * @param v the vector to multiply with the matrix.
         * @return Vector3 the resulting vector after multiplication.
         */
        Vector3 MultiplyPoint(const Vector3& v) const
        {
            float x = data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z + data[0][3];
            float y = data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z + data[1][3];
            float z = data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z + data[2][3];
            float w = data[3][0] * v.x + data[3][1] * v.y + data[3][2] * v.z + data[3][3];

            if (w != 0.0f && w != 1.0f)
            {
                x /= w;
                y /= w;
                z /= w;
            }
            return Vector3(x, y, z);
        }

        /**
         * @brief Multiply this matrix by a vector.
         * 
         * @param v the vector to multiply with the matrix.
         * @return Vector3 the resulting vector after multiplication.
         */
        Vector3 MultiplyVector(const Vector3& v) const
        {
            float x = data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z;
            float y = data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z;
            float z = data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z;
            return Vector3(x, y, z);
        }


        /**
         * @brief Create a translation matrix.
         * 
         * @param t the translation vector.
         * @return Mat4 the translation matrix.
         */
        static Mat4 CreateTranslation(const Vector3& t)
        {
            Mat4 result(1.0f);
            result[0][3] = t.x;
            result[1][3] = t.y;
            result[2][3] = t.z;
            return result;
        }
        
        /**
         * @brief Create a rotation matrix from Euler angles (in radians).
         * 
         * @param pitch rotation around the X axis.
         * @param yaw rotation around the Y axis.
         * @param roll rotation around the Z axis.
         * @return Mat4 the rotation matrix.
         */
        static Mat4 CreateScale(const Vector3& s)
        {
            Mat4 result(1.0f);
            result[0][0] = s.x;
            result[1][1] = s.y;
            result[2][2] = s.z;
            return result;
        }
        /**
         * @brief Create a rotation matrix from Euler angles (in radians).
         * 
         * @param pitch rotation around the X axis.
         * @param yaw rotation around the Y axis.
         * @param roll rotation around the Z axis.
         * @return Mat4 the rotation matrix.
         */        
        static Mat4 CreateFromEulerAngles(float pitch, float yaw, float roll)
        {
            // Create rotation matrices for each axis
            float cp = cosf(pitch), sp = sinf(pitch);
            float cy = cosf(yaw), sy = sinf(yaw);
            float cr = cosf(roll), sr = sinf(roll);
        
            Mat4 rx(1.0f);
            rx[1][1] = cp; rx[1][2] = -sp;
            rx[2][1] = sp; rx[2][2] = cp;
        
            Mat4 ry(1.0f);
            ry[0][0] = cy; ry[0][2] = sy;
            ry[2][0] = -sy; ry[2][2] = cy;
        
            Mat4 rz(1.0f);
            rz[0][0] = cr; rz[0][1] = -sr;
            rz[1][0] = sr; rz[1][1] = cr;
        
            return rz * ry * rx; // Note: order matters!
        }

    };
}


#endif