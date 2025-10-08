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

#include "PulseEngine/core/Meshes/RenderableMesh.h"

struct Bone
{
    std::string name;                 // Bone name, matches aiBone->mName
    int index;                        // Bone index in the skeleton (for GPU)
    int parentIndex;                  // Index of parent bone (-1 if root)
    
    PulseEngine::Mat4 offsetMatrix;   // Inverse bind pose
    PulseEngine::Mat4 localTransform; // From animation (position/rot/scale composed)
    PulseEngine::Mat4 globalTransform; // Computed world-space transform

    Bone() : index(-1), parentIndex(-1) {}
};

struct PULSE_ENGINE_DLL_API TransformAnimation 
{
    PulseEngine::Vector3 position;
    PulseEngine::Vector3 scale;
    PulseEngine::Vector4 rotation; // quaternion (x, y, z, w)
};

struct PULSE_ENGINE_DLL_API KeyFrame 
{
    double time; // in ticks or seconds
    std::unordered_map<std::string, TransformAnimation> boneTransforms;
};

struct PULSE_ENGINE_DLL_API AnimationClip 
{
    std::string name;
    std::vector<KeyFrame> keyframes;
    double duration;
    int tickPerSeconds;
};

class PULSE_ENGINE_DLL_API SkeletalMesh : public RenderableMesh
{
    public:
    SkeletalMesh(const std::string& name) : RenderableMesh(name) {}

    void Update() override;
    void Render(Shader* shader) const override;
    const KeyFrame* FindKeyframeAtTime(double time);

    static AnimationClip LoadAnimationSimplified(const aiAnimation* anim);
    static PulseEngine::Mat4 ConvertAiMatrix(const aiMatrix4x4& from);
    static TransformAnimation InterpolateBoneAtTime(aiNodeAnim* channel, double time);

    std::vector<AnimationClip> animations;
    std::vector<Bone> skeleton;
    std::vector<PulseEngine::Mat4> finalBoneMatrices;
    std::unordered_map<std::string, int> boneNameToIndex;
    private:
    int actualAnimationIndex = 0;

    float internalClock = 0.0f;
};

#endif // SKELETAL_MESH_H
