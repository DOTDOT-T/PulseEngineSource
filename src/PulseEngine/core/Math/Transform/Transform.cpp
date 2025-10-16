#include "Transform.h"
#include "PulseEngine/core/Math/MathUtils.h" // For degrees to radians conversion
#include "PulseEngine/core/FileManager/Archive/Archive.h"

namespace PulseEngine 
{
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

    Transform::Transform(const Vector3& pos, const Vector3& rot, const Vector3& scl)
        : position(pos), rotation(rot), scale(scl)
    {
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


    Vector3 Transform::GetUp() const
    {
        Mat4 rotationMatrix = MathUtils::CreateRotationMatrix(rotation);
        return Vector3(rotationMatrix[1][0], rotationMatrix[1][1], rotationMatrix[1][2]).Normalized();
    }

}
