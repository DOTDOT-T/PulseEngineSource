#ifndef CASTING_H
#define CASTING_H

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/Math/Vector.h"
#include "common/dllExport.h"

class Entity;
class Collider;

namespace PulseEngine::Physics
{
    struct PULSE_ENGINE_DLL_API CastResult
    {
        Entity* hitEntity;
        Collider* hitCollider;
        PulseEngine::Vector3 impactLocation;

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
        float step = 0.1f;
        float gravity = 0.0f;
    };

    class PULSE_ENGINE_DLL_API Casting : public PulseEngine::Registry::PulseObject
    {
        PULSE_GEN_BODY(Casting)
        PULSE_REGISTER_CLASS_HEADER(Casting)
    public:
        static PulseEngine::Physics::CastResult Cast(const PulseEngine::Physics::CastData& castData); 

    private:
    };
}

#endif