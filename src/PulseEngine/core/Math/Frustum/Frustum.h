#ifndef FRUSTUM_H
#define FRUSTUM_H

// #include "Common/common.h"

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/Math/Frustum/AABB.h"
#include "Common/dllExport.h"

#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/core/Math/Mat4.h"

struct PULSE_ENGINE_DLL_API Plane : public PulseObject
{
    PULSE_GEN_BODY(Plane)
    PULSE_REGISTER_CLASS_HEADER(Plane)
    PulseEngine::Vector3 normal;
    float d;

    Plane() = default;
    Plane(const PulseEngine::Vector3& n, float distance) : normal(n), d(distance) {}

    // signed distance from point to plane
    float DistanceToPoint(const PulseEngine::Vector3& p) const;
};

struct PULSE_ENGINE_DLL_API Frustum : public PulseObject
{
    PULSE_GEN_BODY(Frustum)
    PULSE_REGISTER_CLASS_HEADER(Frustum)
    Plane planes[6]; // left, right, top, bottom, near, far

    enum { LEFT = 0, RIGHT, TOP, BOTTOM, NEAR_P, FAR_P };

    // Extracts the frustum planes from a ViewProjection matrix
    void ExtractFromMatrix(const PulseEngine::Mat4& viewProj);

    // Check if an AABB is inside or intersecting the frustum
    bool IntersectsAABB(const AABB& box) const;
};


#endif