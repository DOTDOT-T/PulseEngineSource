#include "Transform.h"
#include "PulseEngine/core/Math/MathUtils.h" // For degrees to radians conversion
#include "PulseEngine/core/FileManager/Archive/Archive.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"

namespace PulseEngine 
{

    PULSE_REGISTER_CLASS_CPP(Transform)

    void Transform::Serialize(Archive &ar)
    {
        ar.Serialize("transform.position.x", position.x);
        ar.Serialize("transform.position.y", position.y);
        ar.Serialize("transform.position.z", position.z);
        ar.Serialize("transform.rotation.x", rotation.x);
        ar.Serialize("transform.rotation.y", rotation.y);
        ar.Serialize("transform.rotation.z", rotation.z);
        ar.Serialize("transform.scale.x", scale.x);
        ar.Serialize("transform.scale.y", scale.y);
        ar.Serialize("transform.scale.z", scale.z);
    }

    void Transform::Deserialize(Archive &ar)
    {
    }

    const char* Transform::ToString()
    {
        return "transform";
    }

    Transform::Transform(const Vector3& pos, const Vector3& rot, const Vector3& scl)
        : position(pos), rotation(rot), scale(scl)
    {    
        EDITOR_LOG("transform construct")
    }

    Vector3 Transform::Apply(const Vector3& point) const 
    {
        // Apply scale
        Vector3 scaled = Vector3(point.x * scale.x, point.y * scale.y, point.z * scale.z);

        // Convert rotation degrees to radians
        float pitch = MathUtils::ToRadians(rotation.x);
        float yaw   = MathUtils::ToRadians(rotation.y);
        float roll  = MathUtils::ToRadians(rotation.z);

        // Rotation matrix from Euler angles
        Mat4 rotationMatrix = Mat4::CreateFromEulerAngles(pitch, yaw, roll);

        // Apply rotation
        Vector3 rotated = rotationMatrix.MultiplyPoint(scaled);

        // Apply translation
        return rotated + position;
    }

    Vector3 Transform::GetForward() const
    {
        Mat4 rotationMatrix = MathUtils::CreateRotationMatrix(rotation);
        // Forward is the -Z axis in most right-handed systems
        return Vector3(-rotationMatrix[2][0], -rotationMatrix[2][1], -rotationMatrix[2][2]).Normalized();
    }

    Vector3 Transform::GetRight() const
    {
        Mat4 rotationMatrix = MathUtils::CreateRotationMatrix(rotation);
        return Vector3(rotationMatrix[0][0], rotationMatrix[0][1], rotationMatrix[0][2]).Normalized();
    }

void Transform::AddWorldRotation(const Vector3& deltaEulerDeg)
{
    using namespace PulseEngine;

    Vector3 deltaEulerRad = deltaEulerDeg * (M_PI / 180.0f);
    Vector3 localEulerRad = rotation * (M_PI / 180.0f);

    Mat3 deltaWorld = Mat3::FromEulerXYZ(deltaEulerRad.x, deltaEulerRad.y, deltaEulerRad.z);
    Mat3 localRot   = Mat3::FromEulerXYZ(localEulerRad.x, localEulerRad.y, localEulerRad.z);

    // Build parent rotation
    Mat3 parentRot = Mat3::Identity();
    if (parent)
    {
        Vector3 parentEulerRad = parent->rotation * (M_PI / 180.0f);
        parentRot = Mat3::FromEulerXYZ(parentEulerRad.x, parentEulerRad.y, parentEulerRad.z);
    }

    Mat3 worldRot = parentRot * localRot;
    Mat3 newWorldRot = deltaWorld * worldRot;

    // Convert back to local space
    Mat3 newLocalRot = parentRot.Inversed() * newWorldRot;
    Vector3 newLocalEuler = Mat3::ToEulerXYZ(newLocalRot);
    newLocalEuler *= (180.0f / M_PI);

    rotation = newLocalEuler;
}


    PulseEngine::Mat4 Transform::GetLocalMatrix()
    {
        Mat4 transformMat = PulseEngine::MathUtils::Matrix::Identity();
        transformMat = PulseEngine::MathUtils::Matrix::Translate(transformMat, position);
        transformMat = PulseEngine::MathUtils::Matrix::RotateZ(transformMat, PulseEngine::MathUtils::ToRadians(rotation.z));
        transformMat = PulseEngine::MathUtils::Matrix::RotateY(transformMat, PulseEngine::MathUtils::ToRadians(rotation.y));
        transformMat = PulseEngine::MathUtils::Matrix::RotateX(transformMat, PulseEngine::MathUtils::ToRadians(rotation.x));
        transformMat = PulseEngine::MathUtils::Matrix::Scale(transformMat, scale);

        return transformMat;
    }

    Vector3 Transform::GetUp() const
    {
        Mat4 rotationMatrix = MathUtils::CreateRotationMatrix(rotation);
        return Vector3(rotationMatrix[1][0], rotationMatrix[1][1], rotationMatrix[1][2]).Normalized();
    }

}
