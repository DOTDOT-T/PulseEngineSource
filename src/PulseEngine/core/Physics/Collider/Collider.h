#ifndef COLLIDER_H
#define COLLIDER_H

#include "PulseEngine/core/PulseEngineBackend.h"
#include "Common/common.h"
#include "PulseEngine/CustomScripts/IScripts.h"


/**
 * @brief Base class for all colliders in the Pulse Engine.
 * @brief it inherit from IScript, it will be needed to attach it to an entity.
 * 
 */
class PULSE_ENGINE_DLL_API Collider : public IScript
{
    public:
        Collider() : IScript() {}
        virtual ~Collider() = default;


        // Implement the required methods from IScript
        void OnStart() override {}
        void OnUpdate() override {}
        void OnRender() override {}
        const char* GetName() const override { return "Collider"; }
        std::vector<ExposedVariable> GetExposedVariables();



        // Collider specific methods
        virtual bool CheckCollision(Collider* other) = 0;
        virtual void ResolveCollision(Collider* other) = 0;

        // Getters and Setters
        virtual PulseEngine::Vector3 GetPosition() const = 0;
        virtual void SetPosition(const PulseEngine::Vector3& position) = 0;

        std::vector<Collider*> othersCollider;
};


#endif