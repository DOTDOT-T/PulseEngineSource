#include "Casting.h"
#include "PulseEngine/core/SceneManager/SceneManager.h"
#include "PulseEngine/core/SceneManager/HierarchyNode.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"
#include "shader.h"
#include "PulseEngine/core/Math/MathUtils.h"

#include <algorithm>

using namespace PulseEngine::Physics;

Shader* PulseEngine::Physics::Casting::lineShader = nullptr;

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

PulseEngine::Physics::Casting::Casting()
{
    if(!lineShader)
    {
        lineShader = new Shader(std::string(ASSET_PATH) + "EngineConfig/shaders/lineTrace.vert", std::string(ASSET_PATH) + "EngineConfig/shaders/lineTrace.frag", PulseEngineGraphicsAPI);
    }
}

PulseEngine::Physics::CastResult *PulseEngine::Physics::Casting::Cast(const PulseEngine::Physics::CastData &castData)
{
    std::vector<Entity*> visibles;
    Vector3 direction = (castData.end - castData.start).Normalized();
    int stepAmount = static_cast<int>(std::ceil((castData.end - castData.start).GetMagnitude() / castData.step));
    Vector3 velocity(0.0f);

    result.start = castData.start;
    result.end = castData.end;

    SceneManager::GetInstance()->GetEntitiesInFrustum(visibles);

    for(int i = 0; i < stepAmount; ++i)
    {    
        Vector3 rayPos = castData.start + direction * (castData.step * i) + velocity;  
        velocity.y -= castData.gravity;

        for(Entity* ent : visibles)
        {
            BoxCollider* bc = ent->collider;

            // TODO : need to be optimize into a map collection name -> entity pointer to easily find the it with the name. go from O(n) to O(1)
            if(std::find(castData.toIgnore.begin(), castData.toIgnore.end(), ent) != castData.toIgnore.end()) continue;
            if(!bc) continue;            
            if(!bc->CheckPositionCollision(rayPos)) continue;

            result.hitCollider = static_cast<Collider*>(bc);
            result.hitEntity = ent;
            result.impactLocation = rayPos;
            return &result; // early-out on first hit
        }
    }

    return &result; // no hit
}

void PulseEngine::Physics::Casting::RenderCast()
{
    if(!PulseEngineGraphicsAPI) return;
    lineShader->Use();
    lineShader->SetMat4("view", PulseEngineInstance->lastView);
    lineShader->SetMat4("projection", PulseEngineInstance->lastProjection);
    lineShader->SetMat4("model", PulseEngine::MathUtils::Matrix::Identity()); 
    lineShader->SetVec3("color", PulseEngine::Vector3(0.0f, 1.0f, 0.0f));
    PulseEngineGraphicsAPI->ActivateWireframe();
    if(result.hitCollider)
    {
        //draw green until we hit a collider. red will be displayed after.
        PulseEngineGraphicsAPI->DrawLine(result.start, result.impactLocation, PulseEngine::Color(0.0f, 1.0f, 0.0f));
        lineShader->SetVec3("color", PulseEngine::Vector3(1.0f, 0.0f, 0.0f));
        PulseEngineGraphicsAPI->DrawLine(result.impactLocation, result.end, PulseEngine::Color(1.0f, 0.0f, 0.0f));
    }
    else
    {
        //only draw green if no collider touch until the end of the trace.
        PulseEngineGraphicsAPI->DrawLine(result.start, result.end, PulseEngine::Color(0.0f, 1.0f, 0.0f));

    }
    PulseEngineGraphicsAPI->DesactivateWireframe();
}
