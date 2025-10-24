#include "PlayerBehavior.h"
#include "PulseEngine/API/MaterialAPI/MaterialApi.h"
#include "PulseEngine/core/Meshes/RenderableMesh.h"
#include <algorithm>
#include <Windows.h>

extern "C" __declspec(dllexport) IScript* PulseScriptPlayerBehavior()
{
    return new PlayerBehavior();
}

void PlayerBehavior::OnStart()
{
    inputSystem = PulseEngineInstance->inputSystem;
    camera = PulseEngineInstance->GetActiveCamera();

    // camera initial placement relative to player
    camera->Position = PulseEngine::Vector3(0.0f, 6.0f, 10.0f);

    // cache meshes once
    wheelMeshes = owner->GetMeshesContainingName("wheel");
    turretMesh = owner->GetMeshContainingName("turret");
    cannonMesh = owner->GetMeshContainingName("cannon");
    chassisMesh = owner->GetMeshContainingName("chassis");

    // store respawn pos
    respawnPosition = owner->GetPosition();

    // initial health
    health = maxHealth;
}

void PlayerBehavior::OnUpdate()
{
    float dt = PulseEngineInstance->GetDeltaTime();

    // timers
    if (invulnerableTimer > 0.0f) invulnerableTimer = std::max(0.0f, invulnerableTimer - dt);
    if (fireTimer > 0.0f) fireTimer = std::max(0.0f, fireTimer - dt);

    UpdateLights(dt);
    HandleMovement(dt);
    HandleTurret(dt);
    HandleCamera(dt);
    HandleShooting(dt);
}

void PlayerBehavior::UpdateLights(float dt)
{
    // déplacer le soleil avec le joueur (un seul directional light)
    for (auto* light : PulseEngineInstance->lights)
    {
        if (auto dir = dynamic_cast<DirectionalLight*>(light))
        {
            sunLight = dir;
            // offset ahead/right relative to player
            sunLight->SetPosition(owner->GetPosition() + PulseEngine::Vector3(5.0f, 5.0f, 0.0f));
            sunLight->target = owner->GetPosition();
        }
    }
}

void PlayerBehavior::HandleMovement(float dt)
{
    // rotations de base du propriétaire
    PulseEngine::Vector3 rot = owner->GetRotation();

    // compute forward from pitch/yaw like original (cache cos/sin)
    float cosPitch = cos(MathUtils::ToRadians(rot.x));
    float sinPitch = sin(MathUtils::ToRadians(rot.x));
    float cosYaw   = cos(MathUtils::ToRadians(rot.y));
    float sinYaw   = sin(MathUtils::ToRadians(rot.y));

    PulseEngine::Vector3 forward(
        cosPitch * sinYaw,
        -sinPitch,
        cosPitch * cosYaw
    );
    forward = forward.Normalized();

    // determine movement input
    PulseEngine::Vector3 inputDir(0.0f);
    bool back = inputSystem->isActionDown(0);
    bool forwardKey = inputSystem->isActionDown(1);
    bool turnRight = inputSystem->isActionDown(2);
    bool turnLeft  = inputSystem->isActionDown(3);

    if (back)  inputDir -= forward;
    if (forwardKey) inputDir += forward;

    // rotation input (yaw)
    if (turnRight) owner->SetRotation(owner->GetRotation() + PulseEngine::Vector3(0.0f, turnSpeedDeg * dt, 0.0f));
    if (turnLeft)  owner->SetRotation(owner->GetRotation() - PulseEngine::Vector3(0.0f, turnSpeedDeg * dt, 0.0f));

    // --- SPEED MANAGEMENT WITH SMOOTH DIRECTION SWITCHING ---
    float targetSpeed = 0.0f;
    PulseEngine::Vector3 targetDir = lastForward;

    if (inputDir.GetMagnitude() > 0.0f)
    {
        targetDir = inputDir.Normalized();
        targetSpeed = maxSpeed;
    }

    // dot product to detect direction conflict (opposite input)
    float dirDot = lastForward.Dot(targetDir);

    if (dirDot < 0.0f && currentSpeed > 0.1f)
    {
        // opposite direction: brake first until stop
        currentSpeed = std::max(0.0f, currentSpeed - deceleration * dt);
    }
    else
    {
        // accelerate or decelerate normally
        if (targetSpeed > currentSpeed)
            currentSpeed = std::min(targetSpeed, currentSpeed + acceleration * dt);
        else
            currentSpeed = std::max(targetSpeed, currentSpeed - deceleration * dt);

        // only update forward vector once almost aligned or moving
        if (inputDir.GetMagnitude() > 0.0f)
            lastForward = targetDir;
    }


    // apply movement
    PulseEngine::Vector3 move = lastForward * currentSpeed * dt;
    owner->GetTransform()->position += move;

    // wheel visuals
    ApplyWheelRotation(currentSpeed * wheelSpinSpeed, dt);

    float dir = 0.0f;
    if (inputSystem->isActionDown(0)) dir = 1.0f;
    if (inputSystem->isActionDown(1)) dir = -1.0f;
    float tiltValue = lastForward.GetMagnitude() * dir * 5.0f;
    chassisMesh->rotation.x = tiltValue;
    turretMesh->rotation.x = tiltValue; 
    cannonMesh->rotation.x += tiltValue; // simple pitch effect based on speed
}

void PlayerBehavior::ApplyWheelRotation(float wheelSpeed, float dt)
{
    // rotate wheels on their local X by an amount proportional to travel
    if (wheelMeshes.empty())
    {
        wheelMeshes = owner->GetMeshesContainingName("wheel");
    }
    for (auto* mesh : wheelMeshes)
    {
        // Smooth rotation; sign based on forward/back keys
        float dir = 0.0f;
        if (inputSystem->isActionDown(0)) dir = -1.0f;
        if (inputSystem->isActionDown(1)) dir = 1.0f;
        mesh->rotation.x += dir * wheelSpeed * dt;
    }
}

float PlayerBehavior::AngleDiff(float target, float current)
{
    float diff = target - current;
    while (diff > 180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;
    return diff;
}

void PlayerBehavior::HandleTurret(float dt)
{
    if (!turretMesh) turretMesh = owner->GetMeshContainingName("turret");
    if (!cannonMesh) cannonMesh = owner->GetMeshContainingName("cannon");
    if (!turretMesh) return;

    // compute yaw to camera (world-space direction on XZ)
    PulseEngine::Vector3 toCam = (camera->Position - owner->GetPosition());
    toCam.y = 0.0f;
    toCam = toCam.Normalized();

    float targetYaw = atan2(toCam.x, toCam.z) * (180.0f / 3.14159265f);

    // current turret yaw is relative to owner rotation; combine to compute world yaw
    float currentTurretYaw = turretMesh->rotation.y + owner->GetRotation().y;
    float yawDiff = AngleDiff(targetYaw, currentTurretYaw);
    float step = yawDiff * Clamp(turretSmooth * dt, 0.0f, 1.0f);

    // rotate both turret and cannon consistently (local rotation)
    turretMesh->rotation.y += step;
    if (cannonMesh)
    {
        cannonMesh->rotation.y = turretMesh->rotation.y;
        // set cannon local position and pitch based on camera pitch
        float yawRad = MathUtils::ToRadians(turretMesh->rotation.y);
        PulseEngine::Vector3 forward;
        forward.x = sin(yawRad);
        forward.y = 0.0f;
        forward.z = cos(yawRad);
        forward = forward.Normalized();

        cannonMesh->position = forward * -1.44f;
        cannonMesh->position.y = 0.94f;

        // clamp camera pitch and apply offset
        float camPitch = Clamp(camera->Pitch, -45.0f, 45.0f);
        cannonMesh->rotation.x = camPitch + cannonPitchOffset;
    }
}

void PlayerBehavior::HandleCamera(float dt)
{
    // mouse look
    float mouseX = PulseEngineInstance->inputSystem->getMouseX();
    float mouseY = PulseEngineInstance->inputSystem->getMouseY();

    // center is hardcoded like before — consider moving to config
    const int centerX = 1920 / 2;
    const int centerY = 1080 / 2;

    camera->ProcessMouseMovement(mouseX - (float)centerX, (float)centerY - mouseY, true);
    SetCursorPos(centerX, centerY);

    // follow player with offset
    camera->Position = owner->GetPosition() - camera->Front * 10.0f + PulseEngine::Vector3(0.0f, 2.5f, 0.0f);
}

void PlayerBehavior::HandleShooting(float dt)
{
    // left mouse or action mapping => shoot
    bool shoot = inputSystem->isActionDown(4) || PulseEngineInstance->inputSystem->wasActionPressed(4); // assuming mouse button API

    if (shoot && fireTimer <= 0.0f && ammo > 0)
    {
        // spawn projectile in front of cannon
        if (cannonMesh)
        {
            // world transform of cannon tip — using position + forward
            float yawRad = MathUtils::ToRadians(cannonMesh->rotation.y + owner->GetRotation().y);
            float pitchRad = MathUtils::ToRadians(cannonMesh->rotation.x);

            PulseEngine::Vector3 dir;
            dir.x = sin(yawRad) * cos(pitchRad);
            dir.y = sin(-pitchRad); // depending on convention, adjust sign
            dir.z = cos(yawRad) * cos(pitchRad);
            dir = dir.Normalized();

            PulseEngine::Vector3 spawnPos = owner->GetPosition() + cannonMesh->position + dir * 1.5f;
            
            // TODO: Replace with your engine's spawn call for a projectile/prefab
            // Example (pseudo): PulseEngineInstance->SpawnProjectile("ShellPrefab", spawnPos, dir * 45.0f, owner);
            // If engine uses EntityFactory: owner->GetScene()->CreateEntityFromPrefab(...)
            
            // Example pseudo-call (no-op stub): 
            // PulseEngine::Entity* proj = PulseEngineInstance->CreateProjectile(spawnPos, dir * 60.0f);
            // proj->SetOwner(owner);

            // feedback: sound/particles (TODO)
            // PulseEngineInstance->audio->Play("cannon_fire");

            ammo--;
            fireTimer = fireCooldown;
        }
    }

    // simple reload when empty (manual or auto)
    if (ammo <= 0 && fireTimer <= 0.0f)
    {
        // auto-reload with delay (example)
        ammo = 20;
        fireTimer = 1.0f;
    }
}

void PlayerBehavior::OnRender()
{
    // debug rendering possible here (health, ammo, cooldown)
    // TODO: use engine debug draw to show HUD elements if available.
}

const char* PlayerBehavior::GetName() const
{
    return "PulseScriptPlayerBehavior";
}
