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