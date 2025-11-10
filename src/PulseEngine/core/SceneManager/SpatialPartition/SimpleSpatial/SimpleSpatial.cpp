#include "SimpleSpatial.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"
#include "PulseEngine/core/Physics/Collider/BoxCollider.h"
#include "PulseEngine/core/Physics/CollisionManager.h"


void SimpleSpatialPartition::Serialize(Archive& ar)
{

}
void SimpleSpatialPartition::Deserialize(Archive& ar)
{

}
const char* SimpleSpatialPartition::ToString()
{
    return "spatial partitionning";
}
void SimpleSpatialPartition::Insert(Entity *entity)
{
    if (!entity) return;
    entities.push_back(entity);
}

void SimpleSpatialPartition::Remove(Entity *entity)
{
    if (!entity) return;
    entities.erase(
        std::remove(entities.begin(), entities.end(), entity),
        entities.end()
    );
}

void SimpleSpatialPartition::Update(Entity * entity)
{
    entity->collider->othersCollider.clear();
    // In a simple flat system, entity transforms are updated elsewhere.
    // Could check if AABB changed and reinsert if needed (future).
}

void SimpleSpatialPartition::Query(const Frustum &frustum, std::vector<Entity *> &outEntities)
    {
        outEntities.clear();
        outEntities.reserve(entities.size());

        for (Entity* e : entities)
        {
                outEntities.push_back(e);
            if (!e) continue;

            AABB aabb = AABB(e->GetPosition() - PulseEngine::Vector3(0.5f), e->GetPosition() + PulseEngine::Vector3(0.5f));
            if (frustum.IntersectsAABB(aabb))
                outEntities.push_back(e);
        }
    }

void SimpleSpatialPartition::DebugDraw()
    {
        // Iterate and visualize entity AABBs (engine-side debug draw call)
        for (Entity* e : entities)
        {
            if (!e) continue;
            // DebugDrawAABB(e->GetWorldAABB(), Color::Green);
        }
    }
