#ifndef COLLIDER_H
#define COLLIDER_H

#include "PulseEngine/core/PulseEngineBackend.h"
#include "Common/common.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/core/Math/Transform/Transform.h"

enum PhysicBody
{
    STATIC,
    MOVABLE
};


/**
 * @brief Base class for all colliders in the Pulse Engine.
 * @brief it inherit from IScript, it will be needed to attach it to an entity.
 * 
 */
class PULSE_ENGINE_DLL_API Collider : public IScript
{
    public:
        Collider() : IScript() 
        {
            AddExposedVariable(EXPOSE_VAR(decalPosition, FLOAT3));
            REGISTER_VAR(decalPosition);
            AddExposedVariable(EXPOSE_VAR(velocity, FLOAT3));
            REGISTER_VAR(velocity);
            AddExposedVariable(EXPOSE_VAR(mass, FLOAT));
            REGISTER_VAR(mass);
            AddExposedVariable(EXPOSE_VAR(physicBody, INT));
            REGISTER_VAR(physicBody);
        }
        virtual ~Collider() = default;


        // Implement the required methods from IScript
        void OnStart() override {}
        void OnUpdate() override {}
        void OnRender() override {}
        void OnEditorDisplay() override {}
        const char* GetName() const override { return "Collider"; }
        std::vector<ExposedVariable> GetExposedVariables();



        // Collider specific methods
        virtual bool CheckCollision(Collider* other) = 0;
        virtual void ResolveCollision(Collider* other) = 0;

        // Getters and Setters
        virtual PulseEngine::Vector3 GetPosition() const = 0;
        virtual void SetPosition(const PulseEngine::Vector3& position) = 0;

        std::vector<Collider*> othersCollider;
        PulseEngine::Transform lastTransform;
        PulseEngine::Vector3 decalPosition;
        PulseEngine::Vector3 velocity;
        PulseEngine::Vector3 force;
        int physicBody = (int)PhysicBody::STATIC;


        float mass = 60.0f;
};


#endif