#include "PhysicAPI.h"
#include "Common/common.h"
#include "PulseEngine/core/Physics/PhysicManager.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/API/EntityAPI/EntityApi.h"

using namespace PulseEngine;


void PhysicAPI::SetBodyType(PulseEngine::EntityApi* ent, bool dynamic)
{
    PulseEngineInstance->physicManager->SetBodyDynamic(ent->entity->bodyID, dynamic);
}

void PhysicAPI::SetBodySize(PulseEngine::EntityApi * ent, const PulseEngine::Vector3 & newHalfExtents)
{
    JPH::Vec3 halfExtents(newHalfExtents.x, newHalfExtents.y, newHalfExtents.z);
    PulseEngineInstance->physicManager->SetBoxSize(ent->entity->bodyID, halfExtents);
}

void PhysicAPI::AddVelocity(PulseEngine::EntityApi *ent, const PulseEngine::Vector3 &velocity)
{
    JPH::Vec3 velDelta(velocity.x, velocity.y, velocity.z);
    PulseEngineInstance->physicManager->AddVelocity(ent->entity->bodyID, velDelta);
}

void PhysicAPI::AddAngularVelocityEuler(PulseEngine::EntityApi* ent, const PulseEngine::Vector3& eulerDegrees)
{
    JPH::Vec3 euler(eulerDegrees.x, eulerDegrees.y, eulerDegrees.z);
    PulseEngineInstance->physicManager->SetAngularVelocityEuler(ent->entity->bodyID, euler);
}
void PhysicAPI::AddAngularVelocityFromVectors(PulseEngine::EntityApi* ent, const PulseEngine::Vector3& start, const PulseEngine::Vector3& end, float factor)
{
    JPH::Vec3 startVec(start.x, start.y, start.z);
    JPH::Vec3 endVec(end.x, end.y, end.z);
    PulseEngineInstance->physicManager->SetAngularVelocityFromVectors(ent->entity->bodyID, startVec, endVec, factor);
}
