#ifndef __CAMERAAPI_H__
#define __CAMERAAPI_H__

#include "Common/dllExport.h"
#include "PulseEngine/core/Math/Vector.h"

class Camera;

/**
 * @class CameraAPI
 * @brief High-level scripting interface for manipulating a camera instance.
 *
 * @details CameraAPI provides access to positional and directional controls that operate
 *  directly on an underlying engine-side Camera object. This abstraction is
 *  designed for game logic, player controllers, AI systems, and runtime camera
 *  behavior authored through custom scripts.
 *  The API exposes essential transformation functions and directional vectors,
 *  enabling precise control over orientation, look-at behavior, and spatial
 *  navigation. All operations reflect immediately in the active camera state.
 */
class PULSE_ENGINE_DLL_API CameraAPI
{
public:

    /**
     * @brief Constructs an API wrapper around an engine Camera instance.
     * @param target Pointer to the camera to control.
     */
    CameraAPI(Camera* target);

    /**
     * @brief Sets the world-space position of the camera.
     * @param pos New position to apply.
     */
    void SetPosition(const PulseEngine::Vector3& pos);

    /**
     * @brief Sets the world-space rotation of the camera.
     * @param rot Euler angles in degrees.
     */
    void SetRotation(const PulseEngine::Vector3& rot);

    /**
     * @brief Reorients the camera to look toward a specific world-space point.
     * @param target The world position the camera should look at.
     */
    void SetLookAt(const PulseEngine::Vector3& target);

    /**
     * @brief Returns the normalized forward direction of the camera.
     * @return Vector3 pointing forward from the camera’s perspective.
     */
    PulseEngine::Vector3 GetFrontVector();

    /**
     * @brief Returns the normalized up direction of the camera.
     * @return Vector3 pointing upward relative to the camera orientation.
     */
    PulseEngine::Vector3 GetUpVector();

    /**
     * @brief Returns the normalized right direction of the camera.
     * @return Vector3 pointing to the camera’s right-hand side.
     */
    PulseEngine::Vector3 GetRightVector();

private:
    /// Pointer to the underlying engine camera instance.
    Camera* owner = nullptr;
};

#endif // __CAMERAAPI_H__
