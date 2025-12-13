#include "PhysicManager.h"

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

    // Exécuter toutes les commandes thread-safe après la simulation
    std::queue<std::unique_ptr<PhysicsCommand>> commandsCopy;
    {
        std::lock_guard<std::mutex> lock(commandQueueMutex);
        std::swap(commandsCopy, commandQueue); // swap pour éviter de garder le lock pendant l'exécution
    }

    std::queue<std::unique_ptr<PhysicsCommand>> remaining;

    while (!commandsCopy.empty())
    {
        auto cmd = std::move(commandsCopy.front()); // move vers une variable locale
        commandsCopy.pop();
        EDITOR_INFO("trying physic command " << typeid(*cmd).name() << ".");

        if(cmd->Execute(this))
        {
            EDITOR_INFO("physic command " << typeid(*cmd).name() << " executed successfully.");
        }
        else
        {
            remaining.push(std::move(cmd)); // remet la commande dans la queue pour retry
        }
    }

    // Remet les commandes restantes dans la queue principale
    if(!remaining.empty())
    {
        EDITOR_INFO("physic command " << typeid(*remaining.front()).name() << "is remaining.");
        std::lock_guard<std::mutex> lock(commandQueueMutex);
        while(!remaining.empty())
        {
            commandQueue.push(std::move(remaining.front()));
            remaining.pop();
        }
    }
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

bool PhysicManager::SetBodyPosition(JPH::BodyID id, const JPH::Vec3 &newPosition)
{
    using namespace JPH;

    // On récupère les vitesses AVANT toute modif
    Vec3 oldLinVel  = bodyInterface->GetLinearVelocity(id);
    Vec3 oldAngVel  = bodyInterface->GetAngularVelocity(id);
    Quat oldRot     = bodyInterface->GetRotation(id);

    // Téléportation via la version qui ne reset pas l'état inertiel
    RVec3 newPos(newPosition.GetX(), newPosition.GetY(), newPosition.GetZ());
    bodyInterface->SetPositionAndRotationWhenChanged(
        id,
        newPos,
        oldRot,
        EActivation::DontActivate
    );

    // On restaure les vitesses du body
    bodyInterface->SetLinearVelocity(id, oldLinVel);
    bodyInterface->SetAngularVelocity(id, oldAngVel);

    // Optionnel : réactiver le body si tu veux qu'il continue la simu immédiatement
    bodyInterface->ActivateBody(id);
    return true;
}

bool PhysicManager::SetBodyRotation(JPH::BodyID id, const JPH::Vec3& eulerAngles)
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
    return true;
}



bool PhysicManager::SetBoxSize(JPH::BodyID id, const JPH::Vec3& newHalfExtents)
{
    RefConst<Shape> newShape = new BoxShape(newHalfExtents);

    bodyInterface->SetShape(id, newShape, true, EActivation::Activate);

    bodyInterface->ActivateBody(id);
    return true;
}

bool PhysicManager::SetBodyDynamic(JPH::BodyID id, bool dynamic)
{
    BodyLockWrite lock(physicsSystem.GetBodyLockInterface(), id);
    if (!lock.Succeeded())
        return false;

    Body& body = lock.GetBody();
    body.SetMotionType(dynamic ? EMotionType::Dynamic : EMotionType::Static);
    return false;
}

bool PhysicManager::AddVelocity(JPH::BodyID id, const JPH::Vec3 & velocityDelta)
{
    RVec3 currentVel = bodyInterface->GetLinearVelocity(id);
    RVec3 newVel = currentVel + RVec3(velocityDelta);
    bodyInterface->SetLinearVelocity(id, newVel);
    JPH::EMotionType motion = bodyInterface->GetMotionType(id);
    const char* motionStr = "Unknown";

    switch(motion)
    {
        case JPH::EMotionType::Static:    motionStr = "Static"; break;
        case JPH::EMotionType::Dynamic:   motionStr = "Dynamic"; break;
        case JPH::EMotionType::Kinematic: motionStr = "Kinematic"; break;
    }

    EDITOR_LOG("New velocity : "
        << newVel.GetX() << " : "
        << newVel.GetY() << " : "
        << newVel.GetZ() << " de body type : "
        << motionStr);

    return true;

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

// ================================================
// SET ANGULAR VELOCITY
// ================================================

// Appliquer une angular velocity depuis des angles Euler en degrés
bool PhysicManager::SetAngularVelocityEuler(JPH::BodyID id, const JPH::Vec3& eulerDegrees)
{
    if (!bodyInterface || id.IsInvalid())
        return false;

    // Convert degrees -> radians
    Vec3 euler = eulerDegrees * (3.14159265359f / 180.0f);

    // Convert Euler XYZ -> quaternion
    JPH::Quat targetRot;
    float cy = cos(euler.GetZ() * 0.5f);
    float sy = sin(euler.GetZ() * 0.5f);
    float cp = cos(euler.GetY() * 0.5f);
    float sp = sin(euler.GetY() * 0.5f);
    float cr = cos(euler.GetX() * 0.5f);
    float sr = sin(euler.GetX() * 0.5f);

    targetRot.SetW(cr*cp*cy + sr*sp*sy);
    targetRot.SetX(sr*cp*cy - cr*sp*sy);
    targetRot.SetY(cr*sp*cy + sr*cp*sy);
    targetRot.SetZ(cr*cp*sy - sr*sp*cy);

    // Calculer la quaternion delta
    JPH::Quat currentRot = bodyInterface->GetRotation(id);
    JPH::Quat deltaRot = targetRot * currentRot.Inversed();
    deltaRot = deltaRot.Normalized();

    // Extraire axis-angle
    float angle = 2.0f * acosf(deltaRot.GetW());
    float s = sqrtf(1.0f - deltaRot.GetW() * deltaRot.GetW());
    Vec3 axis = (s < 0.001f) ? Vec3(1,0,0) : Vec3(deltaRot.GetX()/s, deltaRot.GetY()/s, deltaRot.GetZ()/s);

    // Angular velocity = axis * angle / dt (ici dt = 1/60)
    Vec3 angularVel = axis * (angle / (1/60.0f));

    bodyInterface->SetAngularVelocity(id, RVec3(angularVel));
    bodyInterface->ActivateBody(id);
    return true;
}

// Appliquer angular velocity pour tourner un vecteur start vers end
bool PhysicManager::SetAngularVelocityFromVectors(JPH::BodyID id, const JPH::Vec3& start, const JPH::Vec3& end, float factor)
{
    if (!bodyInterface || id.IsInvalid())
        return false;

    Vec3 v0 = start.Normalized();
    Vec3 v1 = end.Normalized();

    // Calcul quaternion rotation de v0 vers v1
    Vec3 c = v0.Cross(v1);
    float d = v0.Dot(v1);

    if (d >= 1.0f) // vecteurs alignés
    {
        bodyInterface->SetAngularVelocity(id, RVec3(0,0,0));
        return true;
    }
    if (d <= -1.0f) // vecteurs opposés
    {
        Vec3 ortho = Vec3(1,0,0).Cross(v0);
        if (ortho.LengthSq() < 0.0001f)
            ortho = Vec3(0,1,0).Cross(v0);
        ortho = ortho.Normalized();
        bodyInterface->SetAngularVelocity(id, RVec3(ortho * 3.14159265359f / (1/60.0f)));
        return true;
    }

    float s = sqrt((1+d)*2);
    JPH::Quat q(c.GetX()/s, c.GetY()/s, c.GetZ()/s, s/2.0f);
    q = q.Normalized();

    // Axis-angle
    float angle = 2.0f * acosf(q.GetW());
    float axisS = sqrtf(1.0f - q.GetW()*q.GetW());
    Vec3 axis = (axisS < 0.001f) ? Vec3(1,0,0) : Vec3(q.GetX()/axisS, q.GetY()/axisS, q.GetZ()/axisS);

    Vec3 angularVel = axis * (angle / (1/60.0f));
    angularVel /= 100.0f;
    angularVel *= factor;

    bodyInterface->SetAngularVelocity(id, RVec3(angularVel));
    bodyInterface->ActivateBody(id);
    return true;
}



void PhysicManager::EnqueueCommand(std::unique_ptr<PhysicsCommand> cmd)
{
    std::lock_guard<std::mutex> lock(commandQueueMutex);
    commandQueue.push(std::move(cmd));
}
