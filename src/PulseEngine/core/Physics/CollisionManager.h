/**
 * @file CollisionManager.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef COLIISIONMANAGER_H
#define COLIISIONMANAGER_H

#include "common/common.h"

class Collider;

/**
 * @brief For an easy to use backend, the CollisionManager is a static class that manages the collision between two colliders. With that, the "update" method of the engine will not manage collision directly.
 * 
 */
class CollisionManager
{
public:
    CollisionManager() = default;
    ~CollisionManager() = default;

    //manage collision between two colliders
    static void ManageCollision(Collider* collider1, Collider* collider2);

};


#endif // COLIISIONMANAGER_H