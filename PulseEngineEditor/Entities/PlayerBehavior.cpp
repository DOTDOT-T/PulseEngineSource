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
        moveDir -= owner->GetTransform()->GetForward();
    }
    if(inputSystem->isActionDown(1))
    {
        moveDir += owner->GetTransform()->GetForward();
    }
    if(inputSystem->isActionDown(2))
    {
        owner->SetRotation(owner->GetRotation() + PulseEngine::Vector3(0.0f, -25.0f * PulseEngineInstance->GetDeltaTime(), 0.0f));
    }
    if(inputSystem->isActionDown(3))
    {
        owner->SetRotation(owner->GetRotation() - PulseEngine::Vector3(0.0f, -25.0f * PulseEngineInstance->GetDeltaTime(), 0.0f));
    }
    if(moveDir.x != 0.0f || moveDir.z != 0.0f || moveDir.y != 0.0f)
    {
        moveDir = moveDir.Normalized();

        owner->SetPosition(owner->GetPosition() + moveDir * PulseEngineInstance->GetDeltaTime() * 5.0f);
    }

    camera->Front = (owner->GetPosition() - camera->Position).Normalized();   
    
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

