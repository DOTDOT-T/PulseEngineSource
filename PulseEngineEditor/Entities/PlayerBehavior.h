#pragma once

#include "IScripts.h"
#include "Common/dllExport.h"
#include "Engine.h"

#include "PulseEngine/core/Input/InputSystem.h"
#include "PulseEngine/core/Lights/DirectionalLight/DirectionalLight.h"
#include "camera.h"

using namespace PulseLibs;

class PULSE_ENGINE_DLL_API PlayerBehavior : public IScript
{
    public:
        PlayerBehavior()
        {
            // if you need to expose variables, do it here with these macro : 
            //AddExposedVariable(EXPOSE_VAR(speed, INT));
            //REGISTER_VAR(speed); //both are needed to make the variable exposed in the editor
        }
        void OnStart() override;
        void OnUpdate() override;
        void OnRender() override;
        const char* GetName() const override;

private:
    // helpers
    void UpdateLights(float dt);
    void HandleMovement(float dt);
    void HandleTurret(float dt);
    void HandleCamera(float dt);
    void HandleShooting(float dt);
    void ApplyWheelRotation(float wheelSpeed, float dt);

    // math utilities
    static float Lerp(float a, float b, float t) { return a + (b - a) * t; }
    static float Clamp(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
    static float AngleDiff(float target, float current);

private:
    InputSystem* inputSystem = nullptr;
    Camera* camera = nullptr;
    DirectionalLight* sunLight = nullptr;

    // movement parameters (configurable)
    float maxSpeed = 5.0f;          // units / s
    float acceleration = 4.0f;     // units / s^2
    float deceleration = 8.0f;     // units / s^2
    float turnSpeedDeg = 30.0f;     // degrees / s (for manual yaw)
    float wheelSpinSpeed = 80.0f;    // multiplier for wheel rotation

    // smoothing & internal state
    PulseEngine::Vector3 velocity = PulseEngine::Vector3(0.0f);
    PulseEngine::Vector3 lastForward = PulseEngine::Vector3(0.0f, 0.0f, 1.0f);
    float currentSpeed = 0.0f;

    // turret / aim
    float turretSmooth = 8.0f;      // higher = snappier
    float cannonPitchOffset = 20.0f;

    // shooting
    float fireCooldown = 0.25f;
    float fireTimer = 0.0f;
    int ammo = 20;

    // health / respawn
    int maxHealth = 100;
    int health = 100;
    float invulnerableTimer = 0.0f;
    float invulnerableDuration = 2.0f;
    PulseEngine::Vector3 respawnPosition = PulseEngine::Vector3(0.0f, 0.0f, 0.0f);

    // caching
    std::vector<RenderableMesh*> wheelMeshes;
    RenderableMesh* turretMesh = nullptr;
    RenderableMesh* cannonMesh = nullptr;
    RenderableMesh* chassisMesh = nullptr;
};

