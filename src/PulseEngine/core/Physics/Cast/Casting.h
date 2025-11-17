#ifndef CASTING_H
#define CASTING_H

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/Math/Vector.h"
#include "common/dllExport.h"

class Entity;
class Collider;
class Shader;

namespace PulseEngine::Physics
{
    struct PULSE_ENGINE_DLL_API CastResult
    {
        Entity* hitEntity;
        Collider* hitCollider;
        PulseEngine::Vector3 impactLocation;
        PulseEngine::Vector3 start;
        PulseEngine::Vector3 end;

        CastResult()
        {
            hitEntity = nullptr;
            hitCollider = nullptr;
            impactLocation = Vector3(0.0f);
        }
    };

    struct PULSE_ENGINE_DLL_API CastData
    {
        PulseEngine::Vector3 start;
        PulseEngine::Vector3 end;
        std::vector<Entity*> toIgnore;
        float step = 0.1f;
        float gravity = 0.0f;
    };

    class PULSE_ENGINE_DLL_API Casting : public PulseEngine::Registry::PulseObject
    {
        PULSE_GEN_BODY(Casting)
        PULSE_REGISTER_CLASS_HEADER(Casting)
    public:
        Casting();
        virtual PulseEngine::Physics::CastResult* Cast(const PulseEngine::Physics::CastData& castData); 
        void RenderCast();


    private:    
        CastResult result;
        static Shader* lineShader;
        std::vector<Vector3> debugPoints;

    };
}

#endif