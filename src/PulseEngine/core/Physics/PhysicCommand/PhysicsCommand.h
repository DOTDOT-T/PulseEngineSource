#ifndef __PHYSICSCOMMAND_H__
#define __PHYSICSCOMMAND_H__

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

class PhysicManager; // forward declaration

struct PhysicsCommand
{
    virtual ~PhysicsCommand() = default;
    virtual bool Execute(PhysicManager* physicsSystem) = 0;
};

// ============================================
// Commandes concrètes (constructeurs inline)
// ============================================

struct SetBodyPositionCommand : public PhysicsCommand
{
    JPH::BodyID bodyID;
    JPH::Vec3 newPosition;

    // constructeur inline
    SetBodyPositionCommand(JPH::BodyID id, const JPH::Vec3& pos)
        : bodyID(id), newPosition(pos) {}

    bool Execute(PhysicManager* physicsSystem) override;
};

struct SetBodyRotationCommand : public PhysicsCommand
{
    JPH::BodyID bodyID;
    JPH::Vec3 eulerAngles;

    SetBodyRotationCommand(JPH::BodyID id, const JPH::Vec3& angles)
        : bodyID(id), eulerAngles(angles) {}

    bool Execute(PhysicManager* physicsSystem) override;
};

struct AddVelocityCommand : public PhysicsCommand
{
    JPH::BodyID bodyID;
    JPH::Vec3 velocityDelta;

    AddVelocityCommand(JPH::BodyID id, const JPH::Vec3& delta)
        : bodyID(id), velocityDelta(delta) {}

    bool Execute(PhysicManager* physicsSystem) override;
};

struct SetBoxSizeCommand : public PhysicsCommand
{
    JPH::BodyID bodyID;
    JPH::Vec3 newHalfExtents;

    SetBoxSizeCommand(JPH::BodyID id, const JPH::Vec3& halfExtents)
        : bodyID(id), newHalfExtents(halfExtents) {}

    bool Execute(PhysicManager* physicsSystem) override;
};

struct SetAngularVelocityEulerCommand : public PhysicsCommand
{
    JPH::BodyID bodyID;
    JPH::Vec3 eulerDegrees;

    SetAngularVelocityEulerCommand(JPH::BodyID id, const JPH::Vec3& euler)
        : bodyID(id), eulerDegrees(euler) {}

    bool Execute(PhysicManager* physicsSystem) override;
};

struct SetAngularVelocityFromVectorsCommand : public PhysicsCommand
{
    JPH::BodyID bodyID;
    JPH::Vec3 startVector;
    JPH::Vec3 endVector;
    float factor = 1.0f;

    SetAngularVelocityFromVectorsCommand(JPH::BodyID id, const JPH::Vec3& start, const JPH::Vec3& end, float f = 1.0f)
        : bodyID(id), startVector(start), endVector(end), factor(f) {}

    bool Execute(PhysicManager* physicsSystem) override;
};

struct SetBodyDynamicCommand : public PhysicsCommand
{
    JPH::BodyID bodyID;
    bool dynamic;

    SetBodyDynamicCommand(JPH::BodyID id, bool isDynamic)
        : bodyID(id), dynamic(isDynamic) {}

    bool Execute(PhysicManager* physicsSystem) override;
};

#endif // __PHYSICSCOMMAND_H__
