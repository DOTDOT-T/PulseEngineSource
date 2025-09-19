#include "camera.h"

Camera::Camera(PulseEngine::Vector3 position, PulseEngine::Vector3 up, float yaw, float pitch)
    : Front(PulseEngine::Vector3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}

PulseEngine::Mat4 Camera::GetViewMatrix()
{
    // Custom implementation of lookAt using PulseEngine::Vector3
    PulseEngine::Vector3 f = (Position + Front - Position).Normalized();
    PulseEngine::Vector3 s = f.Cross(Up).Normalized();
    PulseEngine::Vector3 u = s.Cross(f);

    PulseEngine::Mat4 result(1.0f);
    result[0][0] = s.x;
    result[1][0] = s.y;
    result[2][0] = s.z;
    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;
    result[0][2] = -f.x;
    result[1][2] = -f.y;
    result[2][2] = -f.z;
    result[3][0] = -s.Dot(Position);
    result[3][1] = -u.Dot(Position);
    result[3][2] = f.Dot(Position);
    return result;
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::UpdateCameraVectors()
{
    PulseEngine::Vector3 front;
    const float DEG2RAD = 3.14159265358979323846f / 180.0f;
    front.x = cos(Yaw * DEG2RAD) * cos(Pitch * DEG2RAD);
    front.y = sin(Pitch * DEG2RAD);
    front.z = sin(Yaw * DEG2RAD) * cos(Pitch * DEG2RAD);
    Front = front.Normalized();
    Right = Front.Cross(WorldUp).Normalized();
    Up    = Right.Cross(Front).Normalized();
}
