#include "EntityApi.h"
#include "PulseEngine/API/MaterialAPI/MaterialApi.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"

PulseEngine::EntityApi::EntityApi(Entity *e) : entity(e), materialApi(new MaterialApi(e->GetMaterial()))
{
}

MaterialApi *PulseEngine::EntityApi::GetMaterialApi()
{
    return materialApi;
}

void PulseEngine::EntityApi::SetColliderSize(const PulseEngine::Vector3 &size)
{
    entity->collider->SetSize(size);
}

void PulseEngine::EntityApi::SetColliderMass(float mass)
{
    entity->collider->mass = mass;
}

void PulseEngine::EntityApi::SetColliderBody(int pb)
{
    entity->collider->physicBody = pb;
}

void PulseEngine::EntityApi::AddVelocity(const PulseEngine::Vector3 &a)
{
    entity->collider->velocity += a;
}

bool PulseEngine::EntityApi::IsVelocityLowerThan(float value)
{
    return entity->collider->velocity.GetMagnitude() < value;
}

void PulseEngine::EntityApi::AddAngularVelocity(const PulseEngine::Vector3 & a)
{
    entity->collider->angularVelocity += a;
}
