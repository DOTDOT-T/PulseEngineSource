#pragma once

#include <cmath>
#include "Common/common.h"

namespace PulseEngine
{

    struct Quaternion
    {
        float w, x, y, z;

        // Constructors
        Quaternion()
            : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}

        Quaternion(float w_, float x_, float y_, float z_)
            : w(w_), x(x_), y(y_), z(z_) {}

        Quaternion(float angleRadians, const Vector3& axis)
        {
            float halfAngle = angleRadians * 0.5f;
            float s = std::sin(halfAngle);

            w = std::cos(halfAngle);
            x = axis.x * s;
            y = axis.y * s;
            z = axis.z * s;
        }

        // Length
        float Length() const
        {
            return std::sqrt(w * w + x * x + y * y + z * z);
        }

        // Normalize
        Quaternion Normalized() const
        {
            float len = Length();
            return Quaternion(w / len, x / len, y / len, z / len);
        }

        // Conjugate
        Quaternion Conjugate() const
        {
            return Quaternion(w, -x, -y, -z);
        }

        // Inverse
        Quaternion Inverse() const
        {
            return Conjugate().Normalized();
        }

        // Multiply (Quaternion * Quaternion)
        Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion(
                w * q.w - x * q.x - y * q.y - z * q.z,
                w * q.x + x * q.w + y * q.z - z * q.y,
                w * q.y - x * q.z + y * q.w + z * q.x,
                w * q.z + x * q.y - y * q.x + z * q.w
            );
        }

        // Rotate a vector
        Vector3 Rotate(const Vector3& v) const
        {
            Quaternion qv(0.0f, v.x, v.y, v.z);
            Quaternion result = (*this) * qv * Inverse();
            return Vector3(result.x, result.y, result.z);
        }

        // To rotation matrix (column-major)
        Mat4 ToMat4() const
        {
            Mat4 mat;

            float xx = x * x;
            float yy = y * y;
            float zz = z * z;
            float xy = x * y;
            float xz = x * z;
            float yz = y * z;
            float wx = w * x;
            float wy = w * y;
            float wz = w * z;

            mat.data[0][0] = 1.0f - 2.0f * (yy + zz);
            mat.data[0][1] = 2.0f * (xy + wz);
            mat.data[0][2] = 2.0f * (xz - wy);
            mat.data[0][3] = 0.0f;

            mat.data[1][0] = 2.0f * (xy - wz);
            mat.data[1][1] = 1.0f - 2.0f * (xx + zz);
            mat.data[1][2] = 2.0f * (yz + wx);
            mat.data[1][3] = 0.0f;

            mat.data[2][0] = 2.0f * (xz + wy);
            mat.data[2][1] = 2.0f * (yz - wx);
            mat.data[2][2] = 1.0f - 2.0f * (xx + yy);
            mat.data[2][3] = 0.0f;

            mat.data[3][0] = 0.0f;
            mat.data[3][1] = 0.0f;
            mat.data[3][2] = 0.0f;
            mat.data[3][3] = 1.0f;

            return mat;
        }

    };

}
