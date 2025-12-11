#ifndef __PHYSICMANAGER_H__
#define __PHYSICMANAGER_H__

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"


class PhysicManager : public PulseObject
{
PULSE_GEN_BODY(PhysicManager)
PULSE_REGISTER_CLASS_HEADER(PhysicManager)

public:
    void InitializePhysicSystem();
    void UpdatePhysicSystem();
    void ShutdownPhysicSystem();

private:
    JPH::PhysicsSystem physicsSystem;
    JPH::TempAllocatorImpl tempAllocator{ 10 * 1024 * 1024 };
    JPH::JobSystemThreadPool jobSystem{ 4, 128 * 1024 };

};
#endif // __PHYSICMANAGER_H__