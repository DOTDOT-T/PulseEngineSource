#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "PulseEngine/core/Physics/Collider/Collider.h"

/**
 * @brief Represents an oriented box collider for 3D physics collision detection.
 * 
 * This class extends the generic Collider interface and uses either standard SAT (Separating Axis Theorem)
 * or a fast approximate method for detecting and resolving collisions.
 */
class PULSE_ENGINE_DLL_API BoxCollider : public Collider
{
public:
    /**
     * @brief Constructs a new BoxCollider with specified position, rotation, and size.
     * @param position Pointer to the position vector.
     * @param rotation Pointer to the rotation vector (in degrees or radians, depending on engine convention).
     * @param size Dimensions of the box along X, Y, Z.
     */
    BoxCollider(PulseEngine::Vector3* position, PulseEngine::Vector3* rotation, const PulseEngine::Vector3& size);

    /**
     * @brief Returns the name of the collider type.
     * @return "BoxCollider".
     */
    const char* GetName() const override
    {
        return "BoxCollider";
    }

    /**
     * @brief Computes the oriented size of the box given a rotation.
     * @param rotation Rotation vector.
     * @param originalSize Original axis-aligned size of the box.
     * @return The size of the box after applying rotation.
     */
    static PulseEngine::Vector3 GetOrientedSize(const PulseEngine::Vector3& rotation, const PulseEngine::Vector3& originalSize);

    /**
     * @brief Checks for collision with another collider.
     * @param other Pointer to the other collider.
     * @return True if there is a collision, false otherwise.
     */
    bool CheckCollision(Collider* other) override;

    /**
     * @brief Performs SAT (Separating Axis Theorem) collision detection with another box.
     * @param otherBox Pointer to the other BoxCollider.
     * @return True if no separation axis is found (i.e. collision), false otherwise.
     */
    bool SeparatedAxisDetection(BoxCollider* otherBox);

    /**
     * @brief Performs a faster, less accurate collision check.
     * @param otherBox Pointer to the other BoxCollider.
     * @return True if the boxes are likely colliding, false otherwise.
     */
    bool FastCheckCollision(BoxCollider* otherBox);

    /**
     * @brief Resolves collision response with another collider.
     * @param other Pointer to the other collider.
     */
    void ResolveCollision(Collider* other) override;

    /**
     * @brief Applies a fast collision resolution method with another box.
     * @param otherBox Pointer to the other BoxCollider.
     */
    void FastResolve(BoxCollider* otherBox);

    /**
     * @brief Returns the current position of the collider.
     * @return The position vector.
     */
    PulseEngine::Vector3 GetPosition() const { return *position; }

    /**
     * @brief Sets the position of the collider.
     * @param pos New position.
     */
    void SetPosition(const PulseEngine::Vector3& pos) { *position = pos; }

    /**
     * @brief Sets the internal position pointer to a new vector.
     * @param posPtr Pointer to the new position vector.
     */
    void SetPointerPosition(PulseEngine::Vector3* posPtr) { position = posPtr; }

    /**
     * @brief Sets the rotation of the collider.
     * @param rot New rotation.
     */
    void SetRotation(const PulseEngine::Vector3& rot) { *rotation = rot; }

    /**
     * @brief Sets the internal rotation pointer to a new vector.
     * @param rotPtr Pointer to the new rotation vector.
     */
    void SetPointerRotation(PulseEngine::Vector3* rotPtr) { rotation = rotPtr; }

    /**
     * @brief Returns the size of the box.
     * @return Size vector (X, Y, Z).
     */
    PulseEngine::Vector3 GetSize() const { return size; }

    /**
     * @brief Sets a new size for the box.
     * @param newSize New size vector.
     */
    void SetSize(const PulseEngine::Vector3& newSize) { size = newSize; }

    /**
     * @brief Returns the half-size of the box (used for bounds and projection).
     * @return Half-size vector.
     */
    PulseEngine::Vector3 GetHalfSize() const;

    /**
     * @brief Returns the local axis direction based on index.
     * @param index Axis index (0 = X, 1 = Y, 2 = Z).
     * @return Unit vector representing the axis.
     */
    PulseEngine::Vector3 GetAxis(int index) const;

    /**
     * @brief Returns the center position of the collider.
     * @return Center vector.
     */
    PulseEngine::Vector3 GetCenter() const;

private:
    PulseEngine::Vector3* position;    ///< Pointer to the position vector.
    PulseEngine::Vector3* rotation;    ///< Pointer to the rotation vector.
    PulseEngine::Vector3 size;         ///< Size of the box (width, height, depth).
    bool isTrigger = false;            ///< If true, only detects collisions but doesn’t resolve them.
    bool hasFastCalculus = false;      ///< If true, uses fast collision detection.
};

#endif // BOXCOLLIDER_H
