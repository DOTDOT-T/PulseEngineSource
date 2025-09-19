#ifndef CAMERA_H
#define CAMERA_H

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include "PulseEngine/core/Math/Vector.h"
#include "PulseEngine/core/Math/Mat4.h"   
#include "Common/dllExport.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class PULSE_ENGINE_DLL_API Camera
{
public:
    PulseEngine::Vector3 Position;
    PulseEngine::Vector3 Front;
    PulseEngine::Vector3 Up;
    PulseEngine::Vector3 Right;
    PulseEngine::Vector3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity = SENSITIVITY;
    float Zoom;

    Camera(
        PulseEngine::Vector3 position = PulseEngine::Vector3(0.0f, 0.0f, 3.0f),
        PulseEngine::Vector3 up = PulseEngine::Vector3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    );

    PulseEngine::Mat4 GetViewMatrix();
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

private:
    void UpdateCameraVectors();
};

#endif
