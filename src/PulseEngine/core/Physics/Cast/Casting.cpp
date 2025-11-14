#include "Casting.h"
#include "PulseEngine/core/SceneManager/SceneManager.h"
#include "PulseEngine/core/SceneManager/HierarchyNode.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/Entity/Entity.h"

using namespace PulseEngine::Physics;
PULSE_REGISTER_CLASS_CPP(Casting)
void PulseEngine::Physics::Casting::Serialize(Archive& ar)
{

}
void PulseEngine::Physics::Casting::Deserialize(Archive& ar)
{

}
const char* PulseEngine::Physics::Casting::ToString()
{
    return "Casting main class";
}

PulseEngine::Physics::CastResult PulseEngine::Physics::Casting::Cast(const PulseEngine::Physics::CastData &castData)
{
    CastResult result;
    std::vector<Entity*> visibles;
    Vector3 direction = (castData.end - castData.start).Normalized();
    int stepAmount = static_cast<int>(std::ceil((castData.end - castData.start).GetMagnitude() / castData.step));

    SceneManager::GetInstance()->GetEntitiesInFrustum(visibles);

    for(int i = 0; i < stepAmount; ++i)
    {    
        Vector3 rayPos = castData.start + direction * (castData.step * i);  

        for(Entity* ent : visibles)
        {
            BoxCollider* bc = ent->collider;
            if(!bc) continue;            
            if(!bc->CheckPositionCollision(rayPos)) continue;

            result.hitCollider = static_cast<Collider*>(bc);
            result.hitEntity = ent;
            result.impactLocation = rayPos;
            return result; // early-out on first hit
        }
    }

    return result; // no hit
}
