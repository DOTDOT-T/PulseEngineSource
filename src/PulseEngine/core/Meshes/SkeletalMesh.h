#ifndef SKELETAL_MESH_H
#define SKELETAL_MESH_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "Common/common.h"
#include "PulseEngine/core/Math/MathUtils.h"

#include "Common/dllExport.h"

/**
 * @brief Contains offset and final transformation matrices for a bone.
 */
struct BoneInfo 
{
    PulseEngine::Mat4 offsetMatrix;     ///< Matrix to transform from model space to bone space.
    PulseEngine::Mat4 finalTransform;   ///< Final transform to apply during skinning.
};

/**
 * @brief Represents a bone with name, ID, and offset transform.
 */
struct Bone
{
    std::string name;           ///< Name of the bone.
    int id;                     ///< Unique ID used in the skeleton.
    PulseEngine::Mat4 offsetMatrix;     ///< Offset matrix from bind pose.
};

/**
 * @brief Represents a single keyframe for a bone in an animation.
 */
struct Keyframe 
{
    float time;                 ///< Time in seconds.
    PulseEngine::Vector3 position;         ///< Position of the bone.
    PulseEngine::Quaternion rotation;         ///< Rotation of the bone.
    PulseEngine::Vector3 scale;            ///< Scaling of the bone.
};

/**
 * @brief Contains all keyframes for a specific bone animation.
 */
struct BoneAnimation 
{
    std::string name;                   ///< Name of the bone.
    std::vector<Keyframe> keyframes;   ///< All keyframes for this bone.
};

/**
 * @brief Represents a complete skeletal structure for skinning and animation.
 * 
 * Handles the bone hierarchy, transformations, and animation playback.
 */
class PULSE_ENGINE_DLL_API Skeleton 
{
public:
    /**
     * @brief Constructs a skeleton from an Assimp mesh and scene.
     * @param mesh Pointer to the Assimp mesh.
     * @param scene Pointer to the Assimp scene.
     */
    Skeleton(const aiMesh* mesh, const aiScene* scene);

    std::map<std::string, int> boneMapping;        ///< Map from bone name to index.
    std::vector<BoneInfo> bones;                   ///< All bone data.
    PulseEngine::Mat4 globalInverseTransform;              ///< Inverse of the root node transform.
    const aiAnimation* animation = nullptr;        ///< Pointer to the animation data.
    aiNode* rootNode;                              ///< Root node of the Assimp scene hierarchy.
    int boneCount = 0;                             ///< Total number of bones.
    std::vector<PulseEngine::Mat4> finalBoneMatrices;      ///< Final transforms to be passed to the vertex shader.

    /**
     * @brief Adds a new bone to the skeleton.
     * @param name Name of the bone.
     * @param offset Offset matrix from the bind pose.
     */
    void AddBone(const std::string& name, const aiMatrix4x4& offset);

    /**
     * @brief Retrieves the offset matrix of a bone by name.
     * @param name Name of the bone.
     * @return The offset matrix of the bone.
     */
    PulseEngine::Mat4 GetBoneOffset(const std::string& name) const;

    /**
     * @brief Applies the animation to the skeleton recursively.
     * @param timeInSeconds Current animation time.
     * @param animation Pointer to the animation data.
     * @param rootNode Root node of the Assimp hierarchy.
     * @param parentTransform Parent transform matrix.
     */
    void ApplyAnimation(float timeInSeconds, const aiAnimation* animation, const aiNode* rootNode, const PulseEngine::Mat4& parentTransform);

    /**
     * @brief Finds the animation channel for a given node.
     * @param animation Pointer to the animation data.
     * @param nodeName Name of the node.
     * @return Pointer to the node's animation channel, or nullptr if not found.
     */
    const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);

    /**
     * @brief Updates the skeleton by advancing the animation.
     * @param deltaTime Time elapsed since the last update.
     */
    void UpdateSkeleton(float deltaTime);

    /**
     * @brief Returns the final bone matrices after animation.
     * @return Vector of final bone transformation matrices.
     */
    const std::vector<PulseEngine::Mat4>& GetFinalBoneMatrices() const;

private:
    /**
     * @brief Converts an Assimp matrix to a glm matrix.
     * @param m Assimp matrix.
     * @return Converted glm matrix.
     */
    PulseEngine::Mat4 ConvertMatrix(const aiMatrix4x4& m);

    /**
     * @brief Interpolates the position for a given animation time.
     * @param time Current animation time.
     * @param channel Pointer to the animation channel.
     * @return Interpolated position.
     */
    PulseEngine::Vector3 InterpolatePosition(float time, const aiNodeAnim* channel);

    /**
     * @brief Interpolates the rotation for a given animation time.
     * @param time Current animation time.
     * @param channel Pointer to the animation channel.
     * @return Interpolated quaternion rotation.
     */
    PulseEngine::Quaternion InterpolateRotation(float time, const aiNodeAnim* channel);

    /**
     * @brief Interpolates the scale for a given animation time.
     * @param time Current animation time.
     * @param channel Pointer to the animation channel.
     * @return Interpolated scale vector.
     */
    PulseEngine::Vector3 InterpolateScaling(float time, const aiNodeAnim* channel);

    float animationTime = 0.0f; ///< Internal time counter for animation playback.
};

#endif // SKELETAL_MESH_H
