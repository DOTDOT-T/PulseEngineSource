#include "EntityApi.h"
#include "PulseEngine/API/MaterialAPI/MaterialApi.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/Physics/PhysicManager.h"
#include "PulseEngine/core/Entity/Entity.h"

PulseEngine::EntityApi::EntityApi(Entity *e) : entity(e), materialApi(new MaterialApi(e->GetMaterial()))
{
};

PulseEngine::Vector3 PulseEngine::EntityApi::GetPosition()
{ 
    return entity->GetPosition(); 
};

PulseEngine::Vector3 PulseEngine::EntityApi::GetRotation()
{ 
    return entity->GetRotation(); 
};

void PulseEngine::EntityApi::SetPosition(const PulseEngine::Vector3 &pos)
{ 
    entity->SetPosition(pos); 
};

void PulseEngine::EntityApi::SetRotation(const PulseEngine::Vector3 &rot)
{ 
    entity->SetRotation(rot); 
};

void PulseEngine::EntityApi::Rotate(const PulseEngine::Vector3 &rot)
{
     entity->SetRotation(entity->GetRotation() + rot);
 };

void PulseEngine::EntityApi::Move(const PulseEngine::Vector3 & move)
{
    entity->transform.position += move;
};

PulseEngine::Transform *PulseEngine::EntityApi::GetTransform()
{ 
    return &entity->transform; 
};

Entity *PulseEngine::EntityApi::GetEntity()
{ 
    return entity;
};
MaterialApi *PulseEngine::EntityApi::GetMaterialApi()
{
    return materialApi;
};

void PulseEngine::EntityApi::SetColliderSize(const PulseEngine::Vector3 &size)
{
    entity->collider->SetSize(size);
};



void PulseEngine::EntityApi::SetColliderMass(float mass)
{
    entity->collider->mass = mass;
};

void PulseEngine::EntityApi::SetColliderBody(int pb)
{
    entity->collider->physicBody = pb;
    PulseEngineInstance->physicManager->SetBodyDynamic(JPH::BodyID(entity->bodyID), (pb != (int)PhysicBody::STATIC));
};

void PulseEngine::EntityApi::AddVelocity(const PulseEngine::Vector3 &a)
{
    entity->collider->velocity += a;
};

bool PulseEngine::EntityApi::IsVelocityLowerThan(float value)
{
    return entity->collider->velocity.GetMagnitude() < value;
};

void PulseEngine::EntityApi::AddAngularVelocity(const PulseEngine::Vector3 & a)
{
    entity->collider->angularVelocity += a;
};
