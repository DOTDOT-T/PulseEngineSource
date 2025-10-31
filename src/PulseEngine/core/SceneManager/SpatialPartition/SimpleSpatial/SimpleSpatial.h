#ifndef SIMPLESPATIAL_H
#define SIMPLESPATIAL_H

#include "PulseEngine/core/SceneManager/SpatialPartition/SpatialPartition.h"
#include "PulseEngine/core/Math/Frustum/Frustum.h"
#include "PulseEngine/core/Entity/Entity.h"

#include <vector>
#include <algorithm>

class Entity;

class PULSE_ENGINE_DLL_API SimpleSpatialPartition : public SpatialPartition
{
    PULSE_GEN_BODY(SimpleSpatialPartition)
    PULSE_REGISTER_CLASS_HEADER(SimpleSpatialPartition)

public:
    SimpleSpatialPartition() = default;
    ~SimpleSpatialPartition() override = default;

    // Insert entity into flat list
    void Insert(Entity* entity) override;

    // Remove entity
    void Remove(Entity* entity) override;

    // Update = no spatial structure here, so nothing to do
    void Update(Entity* entity) override;

    // Query all entities visible in frustum
    void Query(const Frustum& frustum, std::vector<Entity*>& outEntities) override;

    // Optional: draw AABBs for debugging
    void DebugDraw() override;

private:
    std::vector<Entity*> entities;
};

#endif