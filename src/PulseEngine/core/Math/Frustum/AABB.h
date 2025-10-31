#pragma once

#include "Common/common.h"
#include <algorithm>

struct AABB
{
    PulseEngine::Vector3 min;
    PulseEngine::Vector3 max;

    AABB()
        : min( std::numeric_limits<float>::max() ),
          max(-std::numeric_limits<float>::max()) {}

    AABB(const PulseEngine::Vector3& _min, const PulseEngine::Vector3& _max)
        : min(_min), max(_max) {}

    // Expand AABB to include another AABB
    void Expand(const AABB& other)
    {
        min = min.Min(other.min);
        max = max.Max(other.max);
    }

    // Expand to include a point
    void Expand(const PulseEngine::Vector3& point)
    {
        min = min.Min(point);
        max = max.Max(point);
    }

    // Compute the center
    PulseEngine::Vector3 Center() const
    {
        return (min + max) * 0.5f;
    }

    // Compute the extents (half-size)
    PulseEngine::Vector3 Extents() const
    {
        return (max - min) * 0.5f;
    }

    // Compute full size
    PulseEngine::Vector3 Size() const
    {
        return (max - min);
    }

    // Test if it overlaps another AABB
    bool Intersects(const AABB& other) const
    {
        if (max.x < other.min.x || min.x > other.max.x) return false;
        if (max.y < other.min.y || min.y > other.max.y) return false;
        if (max.z < other.min.z || min.z > other.max.z) return false;
        return true;
    }

    // Check if it contains a point
    bool Contains(const PulseEngine::Vector3& p) const
    {
        return (p.x >= min.x && p.x <= max.x) &&
               (p.y >= min.y && p.y <= max.y) &&
               (p.z >= min.z && p.z <= max.z);
    }

    // Transform this AABB by a matrix (to world space)
    AABB Transform(const PulseEngine::Mat4& m) const
    {
        // Extract the corners
        PulseEngine::Vector3 corners[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z}
        };

        AABB result;for (int i = 0; i < 8; i++)
        {
            // Convert corner to 4D homogeneous vector
            PulseEngine::Vector4 corner4(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        
            // Transform by the matrix
            PulseEngine::Vector4 transformed = m * corner4;
        
            // Convert back to 3D (divide by w if your Mat4 supports perspective)
            PulseEngine::Vector3 world(transformed.x, transformed.y, transformed.z);
        
            // Expand the resulting AABB
            result.Expand(world);
        }

        return result;
    }
};
