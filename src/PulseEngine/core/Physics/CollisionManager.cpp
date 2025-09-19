#include "CollisionManager.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"

void CollisionManager::ManageCollision(Collider *collider1, Collider *collider2)
{
    if(collider1 == nullptr || collider2 == nullptr)
    {
        std::cerr << "Error: One or both colliders are null." << std::endl;
        return;
    }

    

    if(collider1->CheckCollision(collider2))
    {
        collider1->othersCollider.push_back(collider2);
        collider2->othersCollider.push_back(collider1);
        collider1->ResolveCollision(collider2);
    }
}