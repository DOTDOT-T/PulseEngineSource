#ifndef __PHYSICMANAGER_H__
#define __PHYSICMANAGER_H__
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

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"

#include "Common/dllExport.h"

#include <thread>
#include <cassert>
#include <mutex>
#include <queue>
#include <memory>

#include "PulseEngine/core/Physics/PhysicCommand/PhysicsCommand.h"



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

    bool SetBodyPosition(JPH::BodyID id, const JPH::Vec3& newPosition);
    bool SetBodyRotation(JPH::BodyID id, const JPH::Vec3& eulerAngles);

    bool SetBoxSize(JPH::BodyID id, const JPH::Vec3& newHalfExtents);
    bool SetBodyDynamic(JPH::BodyID id, bool dynamic);

    bool AddVelocity(JPH::BodyID id, const JPH::Vec3& velocityDelta);

    void EnqueueCommand(std::unique_ptr<PhysicsCommand> cmd);

    bool SetAngularVelocityEuler(JPH::BodyID id, const JPH::Vec3& eulerDegrees);
    bool SetAngularVelocityFromVectors(JPH::BodyID id, const JPH::Vec3& start, const JPH::Vec3& end, float factor = 1.0f);


private:
    static const JPH::ObjectLayer NON_MOVING = 0;
    static const JPH::ObjectLayer MOVING     = 1;

    JPH::PhysicsSystem physicsSystem;

    std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;

    JPH::BodyInterface* bodyInterface = nullptr;

    
    std::mutex commandQueueMutex;
    std::queue<std::unique_ptr<PhysicsCommand>> commandQueue;
};

#endif
