#include "SkeletalMesh.h"


Skeleton::Skeleton(const aiMesh *mesh, const aiScene *scene)
{
    boneCount = 0;

    rootNode = scene->mRootNode;
    if (scene->mNumAnimations > 0)
    {
        animation = scene->mAnimations[0];
    }
    else
    {
        animation = nullptr;
        std::cerr << "[Skeleton] No animations found in scene." << std::endl;
    }
    
    for (unsigned int i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* aiBone = mesh->mBones[i];
        std::string boneName(aiBone->mName.C_Str());

        AddBone(boneName, aiBone->mOffsetMatrix);
    }
    globalInverseTransform = PulseEngine::MathUtils::Matrix::Inverse(ConvertMatrix(scene->mRootNode->mTransformation));
    finalBoneMatrices.resize(100, PulseEngine::Mat4(1.0f));

}

void Skeleton::AddBone(const std::string &name, const aiMatrix4x4 &offset)
{
    if (boneMapping.find(name) == boneMapping.end()) {
        boneMapping[name] = boneCount++;
        BoneInfo bone;
        bone.offsetMatrix = ConvertMatrix(offset);
        bones.push_back(bone);
    }
}

PulseEngine::Mat4 Skeleton::GetBoneOffset(const std::string &name) const
{
    auto it = boneMapping.find(name);
    if (it != boneMapping.end()) {
        return bones[it->second].offsetMatrix;
    }
    return PulseEngine::Mat4(1.0f);
}

void Skeleton::ApplyAnimation(float timeInSeconds, const aiAnimation* animation, const aiNode* node, const PulseEngine::Mat4& parentTransform)
{
    std::string nodeName(node->mName.C_Str());

    const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);

    PulseEngine::Mat4 nodeTransform = ConvertMatrix(node->mTransformation);

    if (nodeAnim)
    {
        PulseEngine::Vector3 scaling = InterpolateScaling(timeInSeconds, nodeAnim);
        PulseEngine::Quaternion rotation = InterpolateRotation(timeInSeconds, nodeAnim);
        PulseEngine::Vector3 position = InterpolatePosition(timeInSeconds, nodeAnim);

        nodeTransform = PulseEngine::MathUtils::Matrix::Translate(PulseEngine::Mat4(1.0f), position)
                      * rotation.ToMat4()
                      * PulseEngine::MathUtils::Matrix::Scale(PulseEngine::Mat4(1.0f), scaling);
    }

    PulseEngine::Mat4 globalTransform = parentTransform * nodeTransform;

    if (boneMapping.find(nodeName) != boneMapping.end())
    {
        int index = boneMapping[nodeName];

        bones[index].finalTransform = globalInverseTransform * globalTransform * bones[index].offsetMatrix;
        PulseEngine::Mat4 offsetMatrix = PulseEngine::MathUtils::Matrix::Transpose(bones[index].offsetMatrix);
        finalBoneMatrices[index] = globalTransform * offsetMatrix;

    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ApplyAnimation(timeInSeconds, animation, node->mChildren[i], globalTransform);
    }
}


const aiNodeAnim* Skeleton::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
{
    if (!animation)
    {
        return nullptr;
    }


    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        aiNodeAnim* channel = animation->mChannels[i];
        if (!channel) continue;
    
        const aiString& nodeNameString = channel->mNodeName;
        if (nodeNameString.length == 0 || !nodeNameString.C_Str()) continue;
    
        std::string animNodeName = nodeNameString.C_Str();
    
        if (nodeName == animNodeName)
        {
            return channel;
        }
    }
    return nullptr;
}


void Skeleton::UpdateSkeleton(float deltaTime)
{
    
    if (!animation)
    {
        return;
    }

    if (!rootNode)
    {
        return;
    }

    animationTime += deltaTime;
    float durationInTicks = animation->mDuration;
    float ticksPerSecond = animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f;

    float timeInTicks = fmod(animationTime * ticksPerSecond, durationInTicks);
    ApplyAnimation(timeInTicks, animation, rootNode, PulseEngine::Mat4(1.0f));
}

const std::vector<PulseEngine::Mat4> &Skeleton::GetFinalBoneMatrices() const
{
    return finalBoneMatrices;
}

PulseEngine::Mat4 Skeleton::ConvertMatrix(const aiMatrix4x4 &m)
{
    return PulseEngine::MathUtils::Matrix::Transpose(PulseEngine::MathUtils::Matrix::MakeMat4(&m.a1));
}

PulseEngine::Vector3 Skeleton::InterpolatePosition(float time, const aiNodeAnim* channel)
{
    if (channel->mNumPositionKeys == 1)
    {
        return PulseEngine::Vector3(channel->mPositionKeys[0].mValue.x,
                         channel->mPositionKeys[0].mValue.y,
                         channel->mPositionKeys[0].mValue.z);
    }

    for (unsigned int i = 0; i < channel->mNumPositionKeys - 1; ++i)
    {
        if (time < static_cast<float>(channel->mPositionKeys[i + 1].mTime))
        {
            float t1 = static_cast<float>(channel->mPositionKeys[i].mTime);
            float t2 = static_cast<float>(channel->mPositionKeys[i + 1].mTime);
            float factor = (time - t1) / (t2 - t1);

            const aiVector3D& start = channel->mPositionKeys[i].mValue;
            const aiVector3D& end = channel->mPositionKeys[i + 1].mValue;

            aiVector3D delta = end - start;
            aiVector3D result = start + factor * delta;

            return PulseEngine::Vector3(result.x, result.y, result.z);
        }
    }

    // fallback (dernier keyframe)
    const aiVector3D& last = channel->mPositionKeys[channel->mNumPositionKeys - 1].mValue;
    return PulseEngine::Vector3(last.x, last.y, last.z);
}


PulseEngine::Quaternion Skeleton::InterpolateRotation(float time, const aiNodeAnim* channel)
{
    if (channel->mNumRotationKeys == 1)
    {
        const aiQuaternion& q = channel->mRotationKeys[0].mValue;
        return PulseEngine::Quaternion(q.w, q.x, q.y, q.z);
    }

    for (unsigned int i = 0; i < channel->mNumRotationKeys - 1; ++i)
    {
        if (time < static_cast<float>(channel->mRotationKeys[i + 1].mTime))
        {
            float t1 = static_cast<float>(channel->mRotationKeys[i].mTime);
            float t2 = static_cast<float>(channel->mRotationKeys[i + 1].mTime);
            float factor = (time - t1) / (t2 - t1);

            const aiQuaternion& start = channel->mRotationKeys[i].mValue;
            const aiQuaternion& end = channel->mRotationKeys[i + 1].mValue;

            aiQuaternion result;
            aiQuaternion::Interpolate(result, start, end, factor);
            result.Normalize();

            return PulseEngine::Quaternion(result.w, result.x, result.y, result.z);
        }
    }

    // fallback
    const aiQuaternion& q = channel->mRotationKeys[channel->mNumRotationKeys - 1].mValue;
    return PulseEngine::Quaternion(q.w, q.x, q.y, q.z);
}

PulseEngine::Vector3 Skeleton::InterpolateScaling(float time, const aiNodeAnim* channel)
{
    if (channel->mNumScalingKeys == 1)
    {
        const aiVector3D& s = channel->mScalingKeys[0].mValue;
        return PulseEngine::Vector3(s.x, s.y, s.z);
    }

    for (unsigned int i = 0; i < channel->mNumScalingKeys - 1; ++i)
    {
        if (time < static_cast<float>(channel->mScalingKeys[i + 1].mTime))
        {
            float t1 = static_cast<float>(channel->mScalingKeys[i].mTime);
            float t2 = static_cast<float>(channel->mScalingKeys[i + 1].mTime);
            float factor = (time - t1) / (t2 - t1);

            const aiVector3D& start = channel->mScalingKeys[i].mValue;
            const aiVector3D& end = channel->mScalingKeys[i + 1].mValue;

            aiVector3D delta = end - start;
            aiVector3D result = start + factor * delta;

            return PulseEngine::Vector3(result.x, result.y, result.z);
        }
    }

    // fallback
    const aiVector3D& last = channel->mScalingKeys[channel->mNumScalingKeys - 1].mValue;
    return PulseEngine::Vector3(last.x, last.y, last.z);
}

