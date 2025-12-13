#include "PhysicsCommand.h"
#include "PulseEngine/core/Physics/PhysicManager.h"

bool SetBodyPositionCommand::Execute(PhysicManager* physicsSystem)
{
    return physicsSystem->SetBodyPosition(bodyID, newPosition);
}

bool SetBodyRotationCommand::Execute(PhysicManager* physicsSystem)
{
    return physicsSystem->SetBodyRotation(bodyID, eulerAngles);
}

bool AddVelocityCommand::Execute(PhysicManager* physicsSystem)
{
    return physicsSystem->AddVelocity(bodyID, velocityDelta);
}

bool SetBoxSizeCommand::Execute(PhysicManager* physicsSystem)
{
    return physicsSystem->SetBoxSize(bodyID, newHalfExtents);
}

bool SetBodyDynamicCommand::Execute(PhysicManager* physicsSystem)
{
    return physicsSystem->SetBodyDynamic(bodyID, dynamic);
}
