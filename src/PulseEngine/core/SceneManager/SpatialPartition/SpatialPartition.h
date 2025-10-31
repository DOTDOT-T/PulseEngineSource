#ifndef SPATIALPARTITION_H
#define SPATIALPARTITION_H

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/Math/Frustum/Frustum.h"

class Entity;

class PULSE_ENGINE_DLL_API SpatialPartition : public PulseObject
{
    PULSE_GEN_BODY(SpatialPartition)
public:
    virtual ~SpatialPartition() = default;

    virtual void Insert(Entity* entity) = 0;

    virtual void Remove(Entity* entity) = 0;

    virtual void Update(Entity* entity) = 0;

    virtual void Query(const Frustum& frustum, std::vector<Entity*>& outEntities) = 0;

    virtual void DebugDraw() {}
};


#endif