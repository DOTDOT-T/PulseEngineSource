#ifndef __PHYSICAPI_H__
#define __PHYSICAPI_H__

#include "Common/dllExport.h"
#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/API/EntityAPI/EntityApi.h"

using BodyID = uint32_t;
using namespace PulseEngine;

class PULSE_ENGINE_DLL_API PhysicAPI
{
public:
    static void SetBodyType(PulseEngine::EntityApi* ent, bool dynamic);
    static void SetBodySize(PulseEngine::EntityApi* ent, const PulseEngine::Vector3& newHalfExtents);

    static void AddVelocity(PulseEngine::EntityApi* ent, const PulseEngine::Vector3& velocity);

private:
};

#endif // __PHYSICAPI_H__