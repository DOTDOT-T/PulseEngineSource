#include "Frustum.h"

PULSE_REGISTER_CLASS_CPP(Plane)
PULSE_REGISTER_CLASS_CPP(Frustum)

float Plane::DistanceToPoint(const PulseEngine::Vector3 &p) const
{
    return normal.Dot(p) + d;
}

void Frustum::ExtractFromMatrix(const PulseEngine::Mat4 & viewProj)
{
    // left
    planes[LEFT].normal.x = viewProj[0][3] + viewProj[0][0];
    planes[LEFT].normal.y = viewProj[1][3] + viewProj[1][0];
    planes[LEFT].normal.z = viewProj[2][3] + viewProj[2][0];
    planes[LEFT].d         = viewProj[3][3] + viewProj[3][0];
    // right
    planes[RIGHT].normal.x = viewProj[0][3] - viewProj[0][0];
    planes[RIGHT].normal.y = viewProj[1][3] - viewProj[1][0];
    planes[RIGHT].normal.z = viewProj[2][3] - viewProj[2][0];
    planes[RIGHT].d         = viewProj[3][3] - viewProj[3][0];
    // top
    planes[TOP].normal.x = viewProj[0][3] - viewProj[0][1];
    planes[TOP].normal.y = viewProj[1][3] - viewProj[1][1];
    planes[TOP].normal.z = viewProj[2][3] - viewProj[2][1];
    planes[TOP].d         = viewProj[3][3] - viewProj[3][1];
    // bottom
    planes[BOTTOM].normal.x = viewProj[0][3] + viewProj[0][1];
    planes[BOTTOM].normal.y = viewProj[1][3] + viewProj[1][1];
    planes[BOTTOM].normal.z = viewProj[2][3] + viewProj[2][1];
    planes[BOTTOM].d         = viewProj[3][3] + viewProj[3][1];
    // near
    planes[NEAR_P].normal.x = viewProj[0][3] + viewProj[0][2];
    planes[NEAR_P].normal.y = viewProj[1][3] + viewProj[1][2];
    planes[NEAR_P].normal.z = viewProj[2][3] + viewProj[2][2];
    planes[NEAR_P].d         = viewProj[3][3] + viewProj[3][2];
    // far
    planes[FAR_P].normal.x = viewProj[0][3] - viewProj[0][2];
    planes[FAR_P].normal.y = viewProj[1][3] - viewProj[1][2];
    planes[FAR_P].normal.z = viewProj[2][3] - viewProj[2][2];
    planes[FAR_P].d         = viewProj[3][3] - viewProj[3][2];
    // normalize all planes
    for (int i = 0; i < 6; i++)
    {
        float length = planes[i].normal.GetMagnitude();
        planes[i].normal = PulseEngine::Vector3(planes[i].normal.x / length,planes[i].normal.y / length,planes[i].normal.z / length);
        planes[i].d /= length;
    }
}

bool Frustum::IntersectsAABB(const AABB & box) const
{
    for (int i = 0; i < 6; ++i)
    {
        const Plane& p = planes[i];
        // Compute positive vertex (furthest in plane normal direction)
        PulseEngine::Vector3 positive = box.min;
        if (p.normal.x >= 0) positive.x = box.max.x;
        if (p.normal.y >= 0) positive.y = box.max.y;
        if (p.normal.z >= 0) positive.z = box.max.z;
        if (p.DistanceToPoint(positive) < 0)
            return false; // box entirely outside
    }
    return true;
}
