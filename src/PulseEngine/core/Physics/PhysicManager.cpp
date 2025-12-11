#include "PhysicManager.h"

void PhysicManager::Serialize(Archive& ar)
{

}
void PhysicManager::Deserialize(Archive& ar)
{

}
const char* PhysicManager::ToString()
{
    return GetTypeName();
}

using namespace JPH;
void PhysicManager::InitializePhysicSystem()
{
    RegisterTypes();
    physicsSystem.Init(1000, &tempAllocator, &jobSystem);
}

void PhysicManager::UpdatePhysicSystem()
{
    float dt = 1.0f / 60.0f; 
    physicsSystem.Update(dt, 1);
}

void PhysicManager::ShutdownPhysicSystem()
{
}
