#include "SkeletalMesh.h"
#include "PulseEngine/core/Meshes/Mesh.h"
#include "PulseEngine/core/Graphics/IGraphicsApi.h"

void SkeletalMesh::Update()
{
    if (actualAnimationIndex >= animations.size()) return;
    internalClock += PulseEngineInstance->GetDeltaTime();

    const AnimationClip& clip = animations[actualAnimationIndex];
    double ticksPerSecond = (clip.tickPerSeconds > 0.0) ? clip.tickPerSeconds : 30.0;

    double timeInTicks = internalClock * ticksPerSecond;
    double animTime = fmod(timeInTicks, clip.duration);

    const KeyFrame* frame = FindKeyframeAtTime(animTime);
    if(!frame) return;

    EDITOR_LOG("animtime : " << animTime << " frame : " << frame->time);

    for (Bone& bone : skeleton)
    {
        auto it = frame->boneTransforms.find(bone.name);
        if (it == frame->boneTransforms.end())
            continue;

        const TransformAnimation& localTransform = it->second;

        PulseEngine::Mat4 local = PulseEngine::MathUtils::Matrix::ComposeMatrix(
            localTransform.position,
            localTransform.rotation,
            localTransform.scale
        );

        PulseEngine::Mat4 global = (bone.parentIndex >= 0)
            ? skeleton[bone.parentIndex].globalTransform * local
            : local;

        bone.globalTransform = global;

        // if (bone.index >= finalBoneMatrices.size())
        //     finalBoneMatrices.resize(skeleton.size());

        finalBoneMatrices[bone.index] = bone.globalTransform * bone.offsetMatrix;
    }
}


void SkeletalMesh::Render(Shader *shader) const
{
    shader->SetBool("hasSkeleton", true);
    PulseEngineGraphicsAPI->SetShaderMat4Array(shader, "u_BoneMatrices", finalBoneMatrices);

    for(Mesh* msh : meshes)
    {
        msh->Draw(shader);
    }
}

AnimationClip SkeletalMesh::LoadAnimationSimplified(const aiAnimation* anim)
{
    AnimationClip clip;
    clip.name = anim->mName.C_Str();

    double duration = anim->mDuration;
    double ticksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 30.0;
    unsigned int frameCount = static_cast<unsigned int>(duration);

    clip.duration = duration;
    clip.tickPerSeconds = ticksPerSecond;

    // Generate per-frame snapshot
    for (unsigned int f = 0; f <= frameCount; f++)
    {
        KeyFrame frame;
        frame.time = (double)f;

        for (unsigned int c = 0; c < anim->mNumChannels; c++)
        {
            aiNodeAnim* channel = anim->mChannels[c];
            std::string boneName = channel->mNodeName.C_Str();

            // Find interpolated transforms for this bone at time f
            aiVector3D pos;
            aiQuaternion rot;
            aiVector3D scale;
            channel->mNumPositionKeys > 0
                ? pos = channel->mPositionKeys[0].mValue
                : pos = aiVector3D(0, 0, 0);
            channel->mNumRotationKeys > 0
                ? rot = channel->mRotationKeys[0].mValue
                : rot = aiQuaternion(1, 0, 0, 0);
            channel->mNumScalingKeys > 0
                ? scale = channel->mScalingKeys[0].mValue
                : scale = aiVector3D(1, 1, 1);

            frame.boneTransforms[boneName] = {
                {pos.x, pos.y, pos.z},
                {scale.x, scale.y, scale.z},
                {rot.x, rot.y, rot.z, rot.w}
            };
        }

        clip.keyframes.push_back(std::move(frame));
    }

    return clip;
}

const KeyFrame* SkeletalMesh::FindKeyframeAtTime(double time)
{
    if (actualAnimationIndex >= animations.size()) return nullptr;
    const AnimationClip& clip = animations[actualAnimationIndex];
    if (clip.keyframes.empty())
        return nullptr;

    // Clamp time to the duration
    if (time <= clip.keyframes.front().time)
        return &clip.keyframes.front();
    if (time >= clip.keyframes.back().time)
        return &clip.keyframes.back();

    // Find the two keyframes between which 'time' falls
    for (size_t i = 0; i < clip.keyframes.size() - 1; ++i)
    {
        const KeyFrame& current = clip.keyframes[i];
        const KeyFrame& next = clip.keyframes[i + 1];
        if (time >= current.time && time <= next.time)
        {
            // For now, just return the current keyframe (no interpolation)
            return &current;

            // Optional: could do linear interpolation here
        }
    }

    return &clip.keyframes.back();
}


PulseEngine::Mat4 SkeletalMesh::ConvertAiMatrix(const aiMatrix4x4& from)
{
    PulseEngine::Mat4 to;
    to.data[0][0] = from.a1; to.data[1][0] = from.a2; to.data[2][0] = from.a3; to.data[3][0] = from.a4;
    to.data[0][1] = from.b1; to.data[1][1] = from.b2; to.data[2][1] = from.b3; to.data[3][1] = from.b4;
    to.data[0][2] = from.c1; to.data[1][2] = from.c2; to.data[2][2] = from.c3; to.data[3][2] = from.c4;
    to.data[0][3] = from.d1; to.data[1][3] = from.d2; to.data[2][3] = from.d3; to.data[3][3] = from.d4;
    return to;
}

TransformAnimation SkeletalMesh::InterpolateBoneAtTime(aiNodeAnim *channel, double time)
{
    TransformAnimation result;

    // // --- Position ---
    // if (channel->mNumPositionKeys == 1)
    //     result.position = PulseEngine::Vector3(channel->mPositionKeys[0].mValue.x,channel->mPositionKeys[0].mValue.y,channel->mPositionKeys[0].mValue.z);
    // else
    // {
    //     for (unsigned int i = 0; i < channel->mNumPositionKeys - 1; ++i)
    //     {
    //         if (time < channel->mPositionKeys[i + 1].mTime)
    //         {
    //             double delta = channel->mPositionKeys[i + 1].mTime - channel->mPositionKeys[i].mTime;
    //             double factor = (time - channel->mPositionKeys[i].mTime) / delta;
    //             result.position = PulseEngine::MathUtils::Lerp(PulseEngine::Vector3(channel->mPositionKeys[i].mValue.x,channel->mPositionKeys[i].mValue.y,channel->mPositionKeys[i].mValue.z),
    //                                    PulseEngine::Vector3(channel->mPositionKeys[i+1].mValue.x,channel->mPositionKeys[i+1].mValue.y,channel->mPositionKeys[i+1].mValue.z),
    //                                    factor);
    //             break;
    //         }
    //     }
    // }

    // // --- Rotation ---
    // if (channel->mNumRotationKeys == 1)
    //     result.rotation = PulseEngine::Vector4(channel->mRotationKeys[0].mValue.x,channel->mRotationKeys[0].mValue.y,channel->mRotationKeys[0].mValue.z,channel->mRotationKeys[0].mValue.w);
    // else
    // {
    //     for (unsigned int i = 0; i < channel->mNumRotationKeys - 1; ++i)
    //     {
    //         if (time < channel->mRotationKeys[i + 1].mTime)
    //         {
    //             double delta = channel->mRotationKeys[i + 1].mTime - channel->mRotationKeys[i].mTime;
    //             double factor = (time - channel->mRotationKeys[i].mTime) / delta;
    //             result.rotation = PulseEngine::MathUtils::Slerp(PulseEngine::Vector4(channel->mRotationKeys[i].mValue.x,channel->mRotationKeys[i].mValue.y,channel->mRotationKeys[i].mValue.z,channel->mRotationKeys[i].mValue.w),
    //                                     PulseEngine::Vector4(channel->mRotationKeys[i+1].mValue.x,channel->mRotationKeys[i+1].mValue.y,channel->mRotationKeys[i+1].mValue.z,channel->mRotationKeys[i+1].mValue.w),
    //                                     factor);
    //             break;
    //         }
    //     }
    // }

    // // --- Scale ---
    // if (channel->mNumScalingKeys == 1)
    //     result.scale = PulseEngine::Vector3(channel->mScalingKeys[0].mValue.x,channel->mScalingKeys[0].mValue.y,channel->mScalingKeys[0].mValue.z);
    // else
    // {
    //     for (unsigned int i = 0; i < channel->mNumScalingKeys - 1; ++i)
    //     {
    //         if (time < channel->mScalingKeys[i + 1].mTime)
    //         {
    //             double delta = channel->mScalingKeys[i + 1].mTime - channel->mScalingKeys[i].mTime;
    //             double factor = (time - channel->mScalingKeys[i].mTime) / delta;
    //             result.scale = PulseEngine::MathUtils::Lerp(PulseEngine::Vector3(channel->mScalingKeys[i].mValue.x,channel->mScalingKeys[i].mValue.y,channel->mScalingKeys[i].mValue.z),
    //                                 PulseEngine::Vector3(channel->mScalingKeys[i+1].mValue.x,channel->mScalingKeys[i+1].mValue.y,channel->mScalingKeys[i+1].mValue.z),
    //                                 factor);
    //             break;
    //         }
    //     }
    // }

    return result;
}

