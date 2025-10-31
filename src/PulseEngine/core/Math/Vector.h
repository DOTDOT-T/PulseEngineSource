#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "Common/dllExport.h"

namespace PulseEngine
{
    /**
     * @brief Vector2 class represents a 2D vector.
     * 
     */
    struct PULSE_ENGINE_DLL_API Vector2
    {
        float x;
        float y;

        Vector2() : x(0), y(0) {}
        Vector2(float scalar) : x(scalar), y(scalar) {}
        Vector2(float x, float y) : x(x), y(y) {}
        Vector2(const Vector2& vect) : x(vect.x), y(vect.y) {}


        Vector2 operator-(const Vector2& other) const
        {
            return Vector2(x - other.x, y - other.y);
        }
    };

    /**
     * @brief Vector3 class represents a 3D vector.
     * 
     */
    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3() : x(0), y(0), z(0) {}
        Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        Vector3(const Vector3& vect) : x(vect.x), y(vect.y), z(vect.z) {}


        
        float& operator[](int index)
        {
            switch (index)
            {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                default: throw std::out_of_range("Index out of range for Vector3");
            }
        }

        // Add assignment operator+=
        Vector3& operator+=(const Vector3& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        // Add assignment operator-=
        Vector3& operator-=(const Vector3& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        // Subtract two vectors
        Vector3 operator-(const Vector3& other) const
        {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        

        // Add two vectors
        Vector3 operator+(const Vector3& other) const
        {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        // Multiply vector by scalar
        Vector3 operator*(float scalar) const
        {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        // Multiply scalar by vector (friend function)
        friend Vector3 operator*(float scalar, const Vector3& vec)
        {
            return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
        }

        float GetMagnitude() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }   


        Vector3 Normalized() const
        {
            float length = std::sqrt(x * x + y * y + z * z);
            if (length == 0.0f) return Vector3(0, 1, 0); // Avoid division by zero
            return Vector3(x / length, y / length, z / length);
        }

        Vector3 Cross(const Vector3& other) const
        {
            return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
            );
        }

        float Dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

            Vector3 Min(const Vector3& other) const
    {
        return Vector3(
            std::min(x, other.x),
            std::min(y, other.y),
            std::min(z, other.z)
        );
    }

    // Returns the component-wise maximum between this vector and another
    Vector3 Max(const Vector3& other) const
    {
        return Vector3(
            std::max(x, other.x),
            std::max(y, other.y),
            std::max(z, other.z)
        );
    }

    // Static helpers (so you can call Vector3::Min(a, b))
    static Vector3 Min(const Vector3& a, const Vector3& b)
    {
        return Vector3(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z)
        );
    }

    static Vector3 Max(const Vector3& a, const Vector3& b)
    {
        return Vector3(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
        );
    }
    };

    struct Vector4
    {
        float x, y, z, a;

        Vector4() : x(0), y(0), z(0), a(0) {}
        Vector4(float scalar) : x(scalar), y(scalar), z(scalar), a(scalar) {}
        Vector4(float ax, float ay, float az, float aa) : x(ax), y(ay), z(az), a(aa) {}
        
        const float& operator[] (int index) const
        {
            switch (index)
            {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                case 3: return a;
                default: throw std::out_of_range("Index out of range for Vector4");
            }
        }

        float& operator[] (int index)
        {
            switch (index)
            {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                case 3: return a;
                default: throw std::out_of_range("Index out of range for Vector4");
            }
        }
    };

    struct iVector4
    {
        int x, y, z, a;

        iVector4() : x(0), y(0), z(0), a(0) {}
        iVector4(int scalar) : x(scalar), y(scalar), z(scalar), a(scalar) {}
        iVector4(int ax, int ay, int az, int aa) : x(ax), y(ay), z(az), a(aa) {}
        
        const int& operator[] (int index) const
        {
            switch (index)
            {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                case 3: return a;
                default: throw std::out_of_range("Index out of range for iVector4");
            }
        }

        int& operator[] (int index)
        {
            switch (index)
            {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                case 3: return a;
                default: throw std::out_of_range("Index out of range for iVector4");
            }
        }

    };
}


#endif
