#include "BoxCollider.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/Material/Material.h"
#define _USE_MATH_DEFINES
#include <cmath>

BoxCollider::BoxCollider(PulseEngine::Vector3* position, PulseEngine::Vector3* rotation, const PulseEngine::Vector3& size)
    : Collider(), size(size)
{
    this->position = position;
    this->rotation = rotation;
}

PulseEngine::Vector3 BoxCollider::GetOrientedSize(const PulseEngine::Vector3& rotation, const PulseEngine::Vector3& originalSize)
{
    using namespace std;
    // Convertit les rotations en radians
    float rotX = rotation.x * (M_PI / 180.0f);
    float rotY = rotation.y * (M_PI / 180.0f);
    float rotZ = rotation.z * (M_PI / 180.0f);

    // Approximation basique : calcule la "bounding box" agrandie à cause de la rotation
    float cosX = std::abs(cos(rotX));
    float sinX = std::abs(sin(rotX));
    float cosY = std::abs(cos(rotY));
    float sinY = std::abs(sin(rotY));
    float cosZ = std::abs(cos(rotZ));
    float sinZ = std::abs(sin(rotZ));

    PulseEngine::Vector3 orientedSize;
    orientedSize.x = originalSize.x * cosY * cosZ + originalSize.y * sinZ + originalSize.z * sinY;
    orientedSize.y = originalSize.y * cosX * cosZ + originalSize.x * sinZ + originalSize.z * sinX;
    orientedSize.z = originalSize.z * cosX * cosY + originalSize.x * sinY + originalSize.y * sinX;

    return orientedSize;
}



bool BoxCollider::CheckCollision(Collider* other)
{
    auto* otherBox = dynamic_cast<BoxCollider*>(other);
    if (!otherBox)
    {
        return false; // Types incompatibles pour l’instant
    }
    if(this->hasFastCalculus) return FastCheckCollision(otherBox);

    return SeparatedAxisDetection(otherBox);
}

#pragma region For BOX collision detection

bool BoxCollider::SeparatedAxisDetection(BoxCollider* otherBox)
{
    PulseEngine::Vector3 centerA = this->GetCenter();
    PulseEngine::Vector3 centerB = otherBox->GetCenter();

    PulseEngine::Vector3 axisA[3] = { GetAxis(0), GetAxis(1), GetAxis(2) };
    PulseEngine::Vector3 axisB[3] = { otherBox->GetAxis(0), otherBox->GetAxis(1), otherBox->GetAxis(2) };

    float ra, rb;
    PulseEngine::Mat3 R, AbsR;

    // Compute rotation matrix expressing boxB in boxA's coordinate frame
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            R[i][j] = PulseEngine::Dot(axisA[i], axisB[j]);
            AbsR[i][j] = std::abs(R[i][j]) + 1e-5f; // epsilon to handle precision
        }
    }

    // Vector from A to B in A's local coordinates
    PulseEngine::Vector3 t(0.0f, 0.0f, 0.0f);
    t.x = centerB.x - centerA.x;
    t.y = centerB.y - centerA.y;
    t.z = centerB.z - centerA.z;
    t = PulseEngine::Vector3(PulseEngine::Dot(t, axisA[0]), PulseEngine::Dot(t, axisA[1]), PulseEngine::Dot(t, axisA[2]));

    PulseEngine::Vector3 halfA = this->GetHalfSize();
    PulseEngine::Vector3 halfB = otherBox->GetHalfSize();

    // Test axes A0, A1, A2
    for (int i = 0; i < 3; ++i)
    {
        ra = halfA[i];
        rb = halfB[0] * AbsR[i][0] + halfB[1] * AbsR[i][1] + halfB[2] * AbsR[i][2];
        if (std::abs(t[i]) > ra + rb) return false;
    }

    // Test axes B0, B1, B2
    for (int i = 0; i < 3; ++i)
    {
        ra = halfA[0] * AbsR[0][i] + halfA[1] * AbsR[1][i] + halfA[2] * AbsR[2][i];
        rb = halfB[i];
        if (std::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            // Compute the axis
            PulseEngine::Vector3 axis = PulseEngine::Cross(axisA[i], axisB[j]);

            // Ignore near-zero vectors (i.e. parallel axes)
            if (axis.x * axis.x + axis.y * axis.y + axis.z * axis.z < 1e-6f)
                continue; // Skip degenerate axis

            axis = PulseEngine::Normalize(axis);

            // Project t onto this axis
            float tProj = std::abs(PulseEngine::Dot(t, axis));

            // Project both boxes onto the axis
            ra = halfA[0] * std::abs(PulseEngine::Dot(axis, axisA[0])) +
                 halfA[1] * std::abs(PulseEngine::Dot(axis, axisA[1])) +
                 halfA[2] * std::abs(PulseEngine::Dot(axis, axisA[2]));

            rb = halfB[0] * std::abs(PulseEngine::Dot(axis, axisB[0])) +
                 halfB[1] * std::abs(PulseEngine::Dot(axis, axisB[1])) +
                 halfB[2] * std::abs(PulseEngine::Dot(axis, axisB[2]));

            if (tProj > ra + rb)
                return false; // Separation axis found
        }
    }
    return true; // No separation axis found
}

bool BoxCollider::FastCheckCollision(BoxCollider *otherBox)
{
    PulseEngine::Vector3 posA = this->GetPosition();
    PulseEngine::Vector3 sizeA = GetOrientedSize(*this->rotation, this->GetSize());

    PulseEngine::Vector3 posB = otherBox->GetPosition();
    PulseEngine::Vector3 sizeB = GetOrientedSize(*otherBox->rotation, otherBox->GetSize());

    sizeA.x /= 2.0f;
    sizeA.y /= 2.0f;
    sizeA.z /= 2.0f;
    sizeB.x /= 2.0f;
    sizeB.y /= 2.0f;
    sizeB.z /= 2.0f;


    bool xOverlap = std::abs(posA.x - posB.x) <= (sizeA.x + sizeB.x);
    bool yOverlap = std::abs(posA.y - posB.y) <= (sizeA.y + sizeB.y);
    bool zOverlap = std::abs(posA.z - posB.z) <= (sizeA.z + sizeB.z);

    bool collisionDetected = xOverlap && yOverlap && zOverlap;

    return xOverlap && yOverlap && zOverlap;
}

#pragma endregion

void BoxCollider::ResolveCollision(Collider* other)
{
    auto* otherBox = dynamic_cast<BoxCollider*>(other);
    if (!otherBox)
    {
        return;
    }

    PulseEngine::Vector3 posA = this->GetPosition();
    PulseEngine::Vector3 sizeA = GetOrientedSize(*this->rotation, this->GetSize());
    PulseEngine::Vector3 posB = otherBox->GetPosition();
    PulseEngine::Vector3 sizeB = GetOrientedSize(*otherBox->rotation, otherBox->GetSize());


    sizeA.x /= 2.0f;
    sizeA.y /= 2.0f;
    sizeA.z /= 2.0f;
    sizeB.x /= 2.0f;
    sizeB.y /= 2.0f;
    sizeB.z /= 2.0f;

    float deltaX = posB.x - posA.x;
    float intersectX = (sizeA.x + sizeB.x) - std::abs(deltaX);

    float deltaY = posB.y - posA.y;
    float intersectY = (sizeA.y + sizeB.y) - std::abs(deltaY);

    float deltaZ = posB.z - posA.z;
    float intersectZ = (sizeA.z + sizeB.z) - std::abs(deltaZ);

    // Trouve le plus petit axe d'intersection
    if (intersectX < intersectY && intersectX < intersectZ)
    {
        float pushX = (deltaX > 0) ? -intersectX : intersectX;
        posA.x += pushX;
    }
    else if (intersectY < intersectZ)
    {
        float pushY = (deltaY > 0) ? -intersectY : intersectY;
        posA.y += pushY;
    }
    else
    {
        float pushZ = (deltaZ > 0) ? -intersectZ : intersectZ;
        posA.z += pushZ;
    }

    this->SetPosition(posA);
}

PulseEngine::Vector3 BoxCollider::GetCenter() const
{
    return *position;
}

PulseEngine::Vector3 BoxCollider::GetHalfSize() const
{
    return PulseEngine::Vector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
}

PulseEngine::Vector3 BoxCollider::GetAxis(int index) const
{
    // Convertir les angles en radians
    float radX = rotation->x * (M_PI / 180.0f);
    float radY = rotation->y * (M_PI / 180.0f);
    float radZ = rotation->z * (M_PI / 180.0f);

    // Matrices de rotation basiques
    PulseEngine::Mat3 rotX = PulseEngine::Mat3::RotationX(radX);
    PulseEngine::Mat3 rotY = PulseEngine::Mat3::RotationY(radY);
    PulseEngine::Mat3 rotZ = PulseEngine::Mat3::RotationZ(radZ);

    // Rotation finale : Y * X * Z
    PulseEngine::Mat3 rotationMatrix = rotY * rotX * rotZ;

    // Axes locaux de base (avant rotation)
    switch (index)
    {
        case 0: return rotationMatrix * PulseEngine::Vector3(1, 0, 0); // X
        case 1: return rotationMatrix * PulseEngine::Vector3(0, 1, 0); // Y
        case 2: return rotationMatrix * PulseEngine::Vector3(0, 0, 1); // Z
        default: return PulseEngine::Vector3(0, 0, 0); // erreur
    }
}

