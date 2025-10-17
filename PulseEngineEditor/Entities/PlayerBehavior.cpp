#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/API/EngineApi.h"
#include "PulseEngine/API/MaterialAPI/MaterialApi.h"
#include "PlayerBehavior.h"
#include "PulseEngine/core/Math/MathUtils.h"

void PlayerBehavior::OnStart()
{
    inputSystem = PulseEngineInstance->inputSystem;
    camera = PulseEngineInstance->GetActiveCamera();
    PulseEngineInstance->GetActiveCamera()->Position = PulseEngine::Vector3(0.0f, 6.0f, 10.0f);
}

void PlayerBehavior::OnUpdate()
{
    PulseEngine::Vector3 moveDir(0.0f);
    if(inputSystem->isActionDown(0))
    {
        moveDir.z -= 1.0f;
    }
    if(inputSystem->isActionDown(1))
    {
        moveDir.z += 1.0f;
    }
    if(inputSystem->isActionDown(2))
    {
        moveDir.x -= 1.0f;
    }
    if(inputSystem->isActionDown(3))
    {
        moveDir.x += 1.0f;
    }
    if(moveDir.x != 0.0f || moveDir.z != 0.0f || moveDir.y != 0.0f)
    {
        moveDir = moveDir.Normalized();

        owner->SetPosition(owner->GetPosition() + moveDir * PulseEngineInstance->GetDeltaTime() * 5.0f);
    }

    camera->Front = (owner->GetPosition() - camera->Position).Normalized();

    owner->GetMaterialApi()->ChangeColor(owner->GetPosition().x / 10.0f + 0.5f, owner->GetPosition().y / 10.0f + 0.5f, owner->GetPosition().z / 10.0f + 0.5f);

    PulseEngine::Vector3 dir = PulseEngine::MathUtils::LookAt(owner->GetPosition(), owner->GetPosition() + moveDir);
    owner->SetRotation(PulseEngine::Vector3(0.0f, PulseEngine::MathUtils::ToDegrees(atan2(dir.x, dir.z)), 0.0f)); // only y rotation
    
    
}

void PlayerBehavior::OnRender()
{
    // Your render code here
}

const char* PlayerBehavior::GetName() const
{
    return "PulseScriptPlayerBehavior";
}

// Register the script
extern "C" __declspec(dllexport) IScript* PulseScriptPlayerBehavior()
{
    return new PlayerBehavior();
}

