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
