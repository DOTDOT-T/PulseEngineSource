#include "CameraAPI.h"
#include "Camera.h"

CameraAPI::CameraAPI(Camera *target) : owner(target)
{
}

void CameraAPI::SetPosition(const PulseEngine::Vector3 &pos)
{
    owner->Position = pos;
}

void CameraAPI::SetRotation(const PulseEngine::Vector3 &rot)
{
    owner->Yaw = rot.x;
    owner->Pitch = rot.y;
    owner->UpdateCameraVectors();
}

void CameraAPI::SetLookAt(const PulseEngine::Vector3 &target)
{
    owner->LookAt(target);
}

PulseEngine::Vector3 CameraAPI::GetFrontVector()
{
    return owner->Front;
}

PulseEngine::Vector3 CameraAPI::GetUpVector()
{
    return owner->Up;
}

PulseEngine::Vector3 CameraAPI::GetRightVector()
{
    return owner->Right;
}
