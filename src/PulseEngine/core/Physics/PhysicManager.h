#ifndef __PHYSICMANAGER_H__
#define __PHYSICMANAGER_H__

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"

#include "Common/dllExport.h"

class PULSE_ENGINE_DLL_API PhysicManager : public PulseObject
{
PULSE_GEN_BODY(PhysicManager)
PULSE_REGISTER_CLASS_HEADER(PhysicManager)

public:
    void InitializePhysicSystem();
    void UpdatePhysicSystem(float dt);
    void ShutdownPhysicSystem();
    
    // === API moteur ===
    JPH::BodyID CreateBox(const JPH::Vec3& pos, const JPH::Vec3& halfExtents, bool dynamic);
    JPH::BodyID CreateSphere(const JPH::Vec3& pos, float radius, bool dynamic);
    
    JPH::Vec3 GetBodyPosition(JPH::BodyID id);
    JPH::Quat GetBodyRotation(JPH::BodyID id);
    void UpdateBodyTransform(JPH::BodyID id, const JPH::Vec3& newPos, const JPH::Quat& newRot);

    void SetBodyPosition(JPH::BodyID id, const JPH::Vec3& newPosition);
    void SetBodyRotation(JPH::BodyID id, const JPH::Vec3& eulerAngles);

    void SetBoxSize(JPH::BodyID id, const JPH::Vec3& newHalfExtents);
    void SetBodyDynamic(JPH::BodyID id, bool dynamic);



private:
    static const JPH::ObjectLayer NON_MOVING = 0;
    static const JPH::ObjectLayer MOVING     = 1;

    JPH::PhysicsSystem physicsSystem;

    std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;

    JPH::BodyInterface* bodyInterface = nullptr;
};

#endif
