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
    static void AddAngularVelocityEuler(PulseEngine::EntityApi* ent, const PulseEngine::Vector3& eulerDegrees);
    static void AddAngularVelocityFromVectors(PulseEngine::EntityApi* ent, const PulseEngine::Vector3& start, const PulseEngine::Vector3& end, float factor = 1.0f);

private:
};

#endif // __PHYSICAPI_H__