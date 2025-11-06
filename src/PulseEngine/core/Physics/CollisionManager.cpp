#include "CollisionManager.h"
#include "PulseEngine/core/Physics/Collider/Collider.h"

void CollisionManager::ManageCollision(Collider *collider1, Collider *collider2)
{
    if(collider1 == nullptr || collider2 == nullptr) return;
    if(collider1 == collider2) return;

    
    if(collider1->CheckCollision(collider2))
    {
        collider1->othersCollider.push_back(collider2);
        collider2->othersCollider.push_back(collider1);
        if(collider1->mass < collider2->mass) collider1->ResolveCollision(collider2);        
        else collider2->ResolveCollision(collider1);
    }

}