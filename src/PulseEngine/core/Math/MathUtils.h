#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "PulseEngine/core/Math/Mat4.h"
#include "PulseEngine/core/Math/Vector.h"
#include <cmath>

namespace PulseEngine
{
    /**
     * @brief Utility functions for mathematical operations.
     * 
     */
    namespace MathUtils
    {
        constexpr float PI = 3.14159265358979323846f;

        inline Vector3 MultiplyPoint(const Mat4& mat, const Vector3& vec)
        {
            return Vector3(
                mat[0][0] * vec.x + mat[0][1] * vec.y + mat[0][2] * vec.z,
                mat[1][0] * vec.x + mat[1][1] * vec.y + mat[1][2] * vec.z,
                mat[2][0] * vec.x + mat[2][1] * vec.y + mat[2][2] * vec.z
            );
        }
        /**
         * @brief Convert degrees to radians.
         * 
         * @param degrees the angle in degrees.
         * @return float the angle in radians.
         */
        inline float ToRadians(float degrees)
        {
            return degrees * (PI / 180.0f);
        }

        /**
         * @brief Convert radians to degrees.
         * 
         * @param radians the angle in radians.
         * @return float the angle in degrees.
         */
        inline float ToDegrees(float radians)
        {
            return radians * (180.0f / PI);
        }

        /**
         * @brief Create a rotation matrix from Euler angles (in radians).
         * 
         * @param euler the Euler angles in radians (pitch, yaw, roll).
         * @return Mat4 the rotation matrix.
         */
        inline Mat4 CreateRotationMatrix(const Vector3& euler)
        {
            float pitch = ToRadians(euler.x); // X-axis
            float yaw   = ToRadians(euler.y); // Y-axis
            float roll  = ToRadians(euler.z); // Z-axis
        
            float cx = cosf(pitch), sx = sinf(pitch);
            float cy = cosf(yaw),   sy = sinf(yaw);
            float cz = cosf(roll),  sz = sinf(roll);
        
            Mat4 mat;
        
            mat[0][0] = cy * cz;
            mat[0][1] = sx * sy * cz - cx * sz;
            mat[0][2] = cx * sy * cz + sx * sz;
            mat[0][3] = 0.0f;
        
            mat[1][0] = cy * sz;
            mat[1][1] = sx * sy * sz + cx * cz;
            mat[1][2] = cx * sy * sz - sx * cz;
            mat[1][3] = 0.0f;
        
            mat[2][0] = -sy;
            mat[2][1] = sx * cy;
            mat[2][2] = cx * cy;
            mat[2][3] = 0.0f;
        
            mat[3][0] = 0.0f;
            mat[3][1] = 0.0f;
            mat[3][2] = 0.0f;
            mat[3][3] = 1.0f;
        
            return mat;
        }

        /**
         * @brief Make an object rotate, to look at a certain position.
         * 
         * @param position of the object
         * @param target we want to look at
         * @param up let it be the default up vector (0,1,0) or change it to another one.
         * @return Vector3 This function returns a Vector3 that represents the direction the object should face.
         */
        inline Vector3 LookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f))
        {
            Vector3 zaxis = (position - target).Normalized(); // Forward vector
            Vector3 xaxis = up.Cross(zaxis).Normalized();    // Right vector
            Vector3 yaxis = zaxis.Cross(xaxis);              // Up vector

            return Vector3(xaxis.x, yaxis.y, zaxis.z);
        }

        /**
         * @brief Give an easy to use function to rotate an point around a target point in a 3D space with no complexity.
         * 
         * @param target The point to orbit around.
         * @param yaw The yaw angle in degrees.
         * @param pitch The pitch angle in degrees.
         * @param radius The distance from the target.
         * @return Vector3 The new position after rotation.
         */
        inline Vector3 RotateAround(const Vector3& target, float yaw, float pitch, float radius)
        {
            float yawRad = ToRadians(yaw);
            float pitchRad = ToRadians(pitch);

            Vector3 offset;
            offset.x = radius * cosf(pitchRad) * cosf(yawRad);
            offset.y = radius * sinf(pitchRad);
            offset.z = radius * cosf(pitchRad) * sinf(yawRad);

            return target - offset;
        }

        
        inline Mat4 PerspectiveMat(float fovRadians, float aspect, float nearPlane, float farPlane)
        {
            float tanHalfFov = tanf(fovRadians / 2.0f);

            Mat4 result = Mat4(0.0f);

            result[0][0] = 1.0f / (aspect * tanHalfFov);
            result[1][1] = 1.0f / (tanHalfFov);
            result[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
            result[2][3] = -1.0f;
            result[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

            return result;
        }

        // inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
        // {
        //     return a * (1.0f - t) + b * t;
        // }
        // inline Quaternion LerpQ(const Quaternion& q1, const Quaternion& q2, float t)
        // {
        //     return Normalize(q1 * (1.0f - t) + q2 * t);
        // }

        // inline Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t)
        // {
        //     float cosTheta = Dot(q1, q2);
        //     Quaternion q2Copy = q2;
        
        //     // If cosTheta < 0, take the shorter path
        //     if (cosTheta < 0.0f) { q2Copy = -q2Copy; cosTheta = -cosTheta; }
        
        //     if (cosTheta > 0.9995f)
        //         return Normalize(LerpQ(q1, q2Copy, t)); // almost identical → linear
        
        //     float angle = acos(cosTheta);
        //     return (sin((1-t)*angle)/sin(angle)) * q1 + (sin(t*angle)/sin(angle)) * q2Copy;
        // }



        namespace Matrix
        {
            static void ToColumnMajor(const PulseEngine::Mat4& m, float out[16])
            {
                for (int row = 0; row < 4; ++row)
                    for (int col = 0; col < 4; ++col)
                        out[col * 4 + row] = m.data[row][col]; // transpose
            }
            
            static PulseEngine::Mat4 FromColumnMajor(const float in[16])
            {
                PulseEngine::Mat4 m;
                for (int row = 0; row < 4; ++row)
                    for (int col = 0; col < 4; ++col)
                        m.data[row][col] = in[col * 4 + row]; // transpose back
                return m;
            }

            inline PulseEngine::Mat4 Identity()
            {
                PulseEngine::Mat4 mat;
                float* m = mat.Ptr();
                memset(m, 0, sizeof(float) * 16);
                m[0] = m[5] = m[10] = m[15] = 1.0f;
                return mat;
            }
            inline PulseEngine::Mat4 Translate(const PulseEngine::Mat4& mat, const PulseEngine::Vector3& pos)
            {
                PulseEngine::Mat4 t = Identity();
                float* m = t.Ptr();
                m[12] = pos.x;
                m[13] = pos.y;
                m[14] = pos.z;
                return mat * t;
            }

            inline PulseEngine::Mat4 RotateX(const PulseEngine::Mat4& mat, float angle)
            {
                PulseEngine::Mat4 rot = Identity();
                float* r = rot.Ptr();
                float c = cos(angle);
                float s = sin(angle);
                r[5] = c;
                r[6] = s;
                r[9] = -s;
                r[10] = c;
                return mat * rot;
            }

            inline PulseEngine::Mat4 RotateY(const PulseEngine::Mat4& mat, float angle)
            {
                PulseEngine::Mat4 rot = Identity();
                float* r = rot.Ptr();
                float c = cos(angle);
                float s = sin(angle);
                r[0] = c;
                r[2] = -s;
                r[8] = s;
                r[10] = c;
                return mat * rot;
            }

            inline PulseEngine::Mat4 RotateZ(const PulseEngine::Mat4& mat, float angle)
            {
                PulseEngine::Mat4 rot = Identity();
                float* r = rot.Ptr();
                float c = cos(angle);
                float s = sin(angle);
                r[0] = c;
                r[1] = s;
                r[4] = -s;
                r[5] = c;
                return mat * rot;
            }

            inline PulseEngine::Mat4 Scale(const PulseEngine::Mat4& mat, const PulseEngine::Vector3& scale)
            {
                PulseEngine::Mat4 result = mat;
                float* m = result.Ptr();
                m[0] *= scale.x;
                m[5] *= scale.y;
                m[10] *= scale.z;
                return result;
            }

            inline Mat4 Orthographic(float left, float right, float bottom, float top, float nearVal, float farVal)
            {
                Mat4 result = Identity();
            
                result[0][0] = 2.0f / (right - left);
                result[1][1] = 2.0f / (top - bottom);
                result[2][2] = -2.0f / (farVal - nearVal);
                result[3][0] = -(right + left) / (right - left);
                result[3][1] = -(top + bottom) / (top - bottom);
                result[3][2] = -(farVal + nearVal) / (farVal - nearVal);
            
                return result;
            }

            inline Mat4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
            {
                Vector3 f = Normalize(center - eye);
                Vector3 s = Normalize(Cross(f, up));
                Vector3 u = Cross(s, f);
            
                Mat4 result = Identity();
                result[0][0] = s.x; result[1][0] = s.y; result[2][0] = s.z;
                result[0][1] = u.x; result[1][1] = u.y; result[2][1] = u.z;
                result[0][2] = -f.x; result[1][2] = -f.y; result[2][2] = -f.z;
                result[3][0] = -Dot(s, eye);
                result[3][1] = -Dot(u, eye);
                result[3][2] = Dot(f, eye);
                return result;
            }


            inline Mat4 Inverse(const Mat4& data)
            {
                Mat4 result;
                const float* m = &data[0][0];
            
                float inv[16], det;
            
                inv[0] = m[5]  * m[10] * m[15] - 
                         m[5]  * m[11] * m[14] - 
                         m[9]  * m[6]  * m[15] + 
                         m[9]  * m[7]  * m[14] +
                         m[13] * m[6]  * m[11] - 
                         m[13] * m[7]  * m[10];
            
                inv[1] = -m[1]  * m[10] * m[15] + 
                          m[1]  * m[11] * m[14] + 
                          m[9]  * m[2] * m[15] - 
                          m[9]  * m[3] * m[14] - 
                          m[13] * m[2] * m[11] + 
                          m[13] * m[3] * m[10];
            
                inv[2] = m[1]  * m[6] * m[15] - 
                         m[1]  * m[7] * m[14] - 
                         m[5]  * m[2] * m[15] + 
                         m[5]  * m[3] * m[14] + 
                         m[13] * m[2] * m[7] - 
                         m[13] * m[3] * m[6];
            
                inv[3] = -m[1]  * m[6] * m[11] + 
                          m[1]  * m[7] * m[10] + 
                          m[5]  * m[2] * m[11] - 
                          m[5]  * m[3] * m[10] - 
                          m[9]  * m[2] * m[7] + 
                          m[9]  * m[3] * m[6];
            
                inv[4] = -m[4]  * m[10] * m[15] + 
                          m[4]  * m[11] * m[14] + 
                          m[8]  * m[6] * m[15] - 
                          m[8]  * m[7] * m[14] - 
                          m[12] * m[6] * m[11] + 
                          m[12] * m[7] * m[10];
            
                inv[5] = m[0]  * m[10] * m[15] - 
                         m[0]  * m[11] * m[14] - 
                         m[8]  * m[2] * m[15] + 
                         m[8]  * m[3] * m[14] + 
                         m[12] * m[2] * m[11] - 
                         m[12] * m[3] * m[10];
            
                inv[6] = -m[0]  * m[6] * m[15] + 
                          m[0]  * m[7] * m[14] + 
                          m[4]  * m[2] * m[15] - 
                          m[4]  * m[3] * m[14] - 
                          m[12] * m[2] * m[7] + 
                          m[12] * m[3] * m[6];
            
                inv[7] = m[0]  * m[6] * m[11] - 
                         m[0]  * m[7] * m[10] - 
                         m[4]  * m[2] * m[11] + 
                         m[4]  * m[3] * m[10] + 
                         m[8]  * m[2] * m[7] - 
                         m[8]  * m[3] * m[6];
            
                inv[8] = m[4]  * m[9] * m[15] - 
                         m[4]  * m[11] * m[13] - 
                         m[8]  * m[5] * m[15] + 
                         m[8]  * m[7] * m[13] + 
                         m[12] * m[5] * m[11] - 
                         m[12] * m[7] * m[9];
            
                inv[9] = -m[0]  * m[9] * m[15] + 
                          m[0]  * m[11] * m[13] + 
                          m[8]  * m[1] * m[15] - 
                          m[8]  * m[3] * m[13] - 
                          m[12] * m[1] * m[11] + 
                          m[12] * m[3] * m[9];
            
                inv[10] = m[0]  * m[5] * m[15] - 
                          m[0]  * m[7] * m[13] - 
                          m[4]  * m[1] * m[15] + 
                          m[4]  * m[3] * m[13] + 
                          m[12] * m[1] * m[7] - 
                          m[12] * m[3] * m[5];
            
                inv[11] = -m[0]  * m[5] * m[11] + 
                           m[0]  * m[7] * m[9] + 
                           m[4]  * m[1] * m[11] - 
                           m[4]  * m[3] * m[9] - 
                           m[8]  * m[1] * m[7] + 
                           m[8]  * m[3] * m[5];
            
                inv[12] = -m[4]  * m[9] * m[14] + 
                           m[4]  * m[10] * m[13] + 
                           m[8]  * m[5] * m[14] - 
                           m[8]  * m[6] * m[13] - 
                           m[12] * m[5] * m[10] + 
                           m[12] * m[6] * m[9];
            
                inv[13] = m[0]  * m[9] * m[14] - 
                          m[0]  * m[10] * m[13] - 
                          m[8]  * m[1] * m[14] + 
                          m[8]  * m[2] * m[13] + 
                          m[12] * m[1] * m[10] - 
                          m[12] * m[2] * m[9];
            
                inv[14] = -m[0]  * m[5] * m[14] + 
                           m[0]  * m[6] * m[13] + 
                           m[4]  * m[1] * m[14] - 
                           m[4]  * m[2] * m[13] - 
                           m[12] * m[1] * m[6] + 
                           m[12] * m[2] * m[5];
            
                inv[15] = m[0]  * m[5] * m[10] - 
                          m[0]  * m[6] * m[9] - 
                          m[4]  * m[1] * m[10] + 
                          m[4]  * m[2] * m[9] + 
                          m[8]  * m[1] * m[6] - 
                          m[8]  * m[2] * m[5];
            
                det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
            
                if (det == 0)
                    throw std::runtime_error("Matrix is singular and cannot be inverted.");
            
                det = 1.0f / det;
            
                for (int i = 0; i < 16; i++)
                    ((float*)result.data)[i] = inv[i] * det;
            
                return result;
            }

            inline Mat4 Transpose(const Mat4& data)
            {
                Mat4 result;
            
                for (int i = 0; i < 4; ++i)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        result.data[i][j] = data[j][i];
                    }
                }
            
                return result;
            }

                inline Mat4 MakeMat4(const float* ptr)
                {
                    Mat4 result;
                
                    for (int col = 0; col < 4; ++col)
                    {
                        for (int row = 0; row < 4; ++row)
                        {
                            result.data[row][col] = ptr[col * 4 + row]; // Column-major interpretation
                        }
                    }
                
                    return result;
                }

                inline PulseEngine::Mat4 ComposeMatrix(const PulseEngine::Vector3& position,
                                                const PulseEngine::Vector4& rotation, // quaternion
                                                const PulseEngine::Vector3& scale)
                {
                    float x = rotation.x, y = rotation.y, z = rotation.z, w = rotation.a;
                    float sx = scale.x, sy = scale.y, sz = scale.z;
                
                    PulseEngine::Mat4 result(1.0f);
                
                    result[0][0] = (1 - 2*y*y - 2*z*z) * sx;
                    result[0][1] = (2*x*y + 2*w*z) * sx;
                    result[0][2] = (2*x*z - 2*w*y) * sx;
                    result[0][3] = 0.0f;
                
                    result[1][0] = (2*x*y - 2*w*z) * sy;
                    result[1][1] = (1 - 2*x*x - 2*z*z) * sy;
                    result[1][2] = (2*y*z + 2*w*x) * sy;
                    result[1][3] = 0.0f;
                
                    result[2][0] = (2*x*z + 2*w*y) * sz;
                    result[2][1] = (2*y*z - 2*w*x) * sz;
                    result[2][2] = (1 - 2*x*x - 2*y*y) * sz;
                    result[2][3] = 0.0f;
                
                    result[3][0] = position.x;
                    result[3][1] = position.y;
                    result[3][2] = position.z;
                    result[3][3] = 1.0f;
                
                    return result;
                }

        }
    }
}


#endif