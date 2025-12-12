#include "PhysicManager.h"
#include <thread>

using namespace JPH;



void PhysicManager::Serialize(Archive& ar)
{

}
void PhysicManager::Deserialize(Archive& ar)
{

}
const char* PhysicManager::ToString()
{
    return GetTypeName();
}

// ================================================
// INIT
// ================================================
void PhysicManager::InitializePhysicSystem()
{
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();
    RegisterTypes();
    tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
    jobSystem     = std::make_unique<JPH::JobSystemThreadPool>(2048, 8, thread::hardware_concurrency() - 1);

    static const ObjectLayer NON_MOVING = 0;
    static const ObjectLayer MOVING     = 1;

    class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
    {
    public:
        bool ShouldCollide(ObjectLayer, ObjectLayer) const override { return true; }
    };

    class BroadPhaseLayerInterfaceImpl : public BroadPhaseLayerInterface
    {
    public:
        BroadPhaseLayerInterfaceImpl()
        {
            mObjToBroad[NON_MOVING] = BroadPhaseLayer(0);
            mObjToBroad[MOVING]     = BroadPhaseLayer(1);
        }

        uint GetNumBroadPhaseLayers() const override { return 2; }

        BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer layer) const override
        {
            return mObjToBroad[layer];
        }

        const char* GetBroadPhaseLayerName(BroadPhaseLayer) const override { return "Default"; }

    private:
        BroadPhaseLayer mObjToBroad[2];
    };

    class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
    {
    public:
        bool ShouldCollide(ObjectLayer, BroadPhaseLayer) const override { return true; }
    };

    static ObjectLayerPairFilterImpl pairFilter;
    static BroadPhaseLayerInterfaceImpl broadphaseInterface;
    static ObjectVsBroadPhaseLayerFilterImpl broadVsObjFilter;

    physicsSystem.Init(
        2000,    // max bodies
        0,       // mutexes
        2048,    // max body pairs
        2048,    // max constraints
        broadphaseInterface,
        broadVsObjFilter,
        pairFilter
    );

    bodyInterface = &physicsSystem.GetBodyInterface();
}

// ================================================
// UPDATE
// ================================================
void PhysicManager::UpdatePhysicSystem(float dt)
{
    physicsSystem.Update(1/60.0f, 1, tempAllocator.get(), jobSystem.get());
}

// ================================================
// SHUTDOWN
// ================================================
void PhysicManager::ShutdownPhysicSystem()
{
    delete Factory::sInstance;
    Factory::sInstance = nullptr;
}

// ================================================
// BODY CREATION (API moteur)
// ================================================
BodyID PhysicManager::CreateBox(const Vec3& pos, const Vec3& halfExtents, bool dynamic)
{
    RefConst<Shape> shape = new BoxShape(halfExtents);

    BodyCreationSettings settings(
        shape,
        pos,
        Quat::sIdentity(),
        dynamic ? EMotionType::Dynamic : EMotionType::Static,
        dynamic ? MOVING : NON_MOVING
    );
    settings.mAllowDynamicOrKinematic = true;

    Body* body = bodyInterface->CreateBody(settings);
    bodyInterface->AddBody(body->GetID(), EActivation::Activate);
    return body->GetID();
}

BodyID PhysicManager::CreateSphere(const Vec3& pos, float radius, bool dynamic)
{
    RefConst<Shape> shape = new SphereShape(radius);

    BodyCreationSettings settings(
        shape,
        pos,
        Quat::sIdentity(),
        dynamic ? EMotionType::Dynamic : EMotionType::Static,
        dynamic ? MOVING : NON_MOVING
    );

    Body* body = bodyInterface->CreateBody(settings);
    bodyInterface->AddBody(body->GetID(), EActivation::Activate);
    return body->GetID();
}

// ================================================
// GET POSITION / ROTATION
// ================================================
Vec3 PhysicManager::GetBodyPosition(BodyID id)
{
    RVec3 pos = bodyInterface->GetPosition(id);
    return Vec3((float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ());
}

Quat PhysicManager::GetBodyRotation(BodyID id)
{
    return bodyInterface->GetRotation(id);
}

void PhysicManager::SetBodyPosition(JPH::BodyID id, const JPH::Vec3 &newPosition)
{
    Quat currentRot = bodyInterface->GetRotation(id);
    RVec3 newPos(newPosition.GetX(), newPosition.GetY(), newPosition.GetZ());
    bodyInterface->SetPosition(id, newPos, EActivation::Activate);
}

void PhysicManager::SetBodyRotation(JPH::BodyID id, const JPH::Vec3& eulerAngles)
{
    // Convert Euler XYZ to quaternion
    Quat q;
    float cy = cos(eulerAngles.GetZ() * 0.5f);
    float sy = sin(eulerAngles.GetZ() * 0.5f);
    float cp = cos(eulerAngles.GetY() * 0.5f);
    float sp = sin(eulerAngles.GetY() * 0.5f);
    float cr = cos(eulerAngles.GetX() * 0.5f);
    float sr = sin(eulerAngles.GetX() * 0.5f);

    q.SetW(cr*cp*cy + sr*sp*sy);
    q.SetX(sr*cp*cy - cr*sp*sy);
    q.SetY(cr*sp*cy + sr*cp*sy);
    q.SetZ(cr*cp*sy - sr*sp*cy);

    bodyInterface->SetRotation(id, q, EActivation::Activate);
}



void PhysicManager::SetBoxSize(JPH::BodyID id, const JPH::Vec3& newHalfExtents)
{
    BodyLockWrite lock(physicsSystem.GetBodyLockInterface(), id);
    if (!lock.Succeeded())
        return;

    Body& body = lock.GetBody();

    RefConst<Shape> newShape = new BoxShape(newHalfExtents);

    bodyInterface->SetShape(id, newShape, true, EActivation::Activate);

    bodyInterface->ActivateBody(id);
}

void PhysicManager::SetBodyDynamic(JPH::BodyID id, bool dynamic)
{
    BodyLockWrite lock(physicsSystem.GetBodyLockInterface(), id);
    if (!lock.Succeeded())
        return;

    Body& body = lock.GetBody();

    if (!body.GetMotionProperties() && dynamic)
    {
        EDITOR_WARN("Cannot make static body dynamic: mAllowDynamicOrKinematic not set at creation\n");
        return;
    }

    EMotionType newType = dynamic ? EMotionType::Dynamic : EMotionType::Static;

    if (body.GetMotionType() != newType)
    {
        body.SetMotionType(newType);
        bodyInterface->ActivateBody(id);
    }
}

void PhysicManager::UpdateBodyTransform(JPH::BodyID id, const JPH::Vec3& newPos, const JPH::Quat& newRot)
{
    if (!bodyInterface || id.IsInvalid())
        return;

    EMotionType motion = bodyInterface->GetMotionType(id);

    if (motion == EMotionType::Static || motion == EMotionType::Kinematic)
    {
        bodyInterface->SetPositionAndRotation(id, RVec3(newPos), newRot, EActivation::DontActivate);
    }
    else
    {
        RVec3 currentPos = bodyInterface->GetPosition(id);
        Quat currentRot  = bodyInterface->GetRotation(id);

        Vec3 linearVel = Vec3(newPos - Vec3(currentPos.GetX(), currentPos.GetY(), currentPos.GetZ())) / (1/60.0f);

        // delta rotation
        Quat deltaRot = newRot * currentRot.Inversed();
        deltaRot = deltaRot.Normalized();

        float angle = 2.0f * acosf(deltaRot.GetW());
        float s = sqrtf(1.0f - deltaRot.GetW() * deltaRot.GetW());
        Vec3 axis = (s < 0.001f) ? Vec3(1,0,0) : Vec3(deltaRot.GetX()/s, deltaRot.GetY()/s, deltaRot.GetZ()/s);

        Vec3 angularVel = axis * (angle / (1/60.0f));

        bodyInterface->SetLinearVelocity(id, RVec3(linearVel));
        bodyInterface->SetAngularVelocity(id, RVec3(angularVel));
        bodyInterface->ActivateBody(id);
    }
}
