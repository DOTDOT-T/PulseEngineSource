#include "GuidReader.h"
#include "Common/common.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "json.hpp"
#include "PulseEngine/core/Meshes/primitive/Primitive.h"
#include "PulseEngine/core/Meshes/Mesh.h"
#include "PulseEngine/core/Material/MaterialManager.h"
#include "PulseEngine/core/FileManager/FileManager.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "PulseEngine/CustomScripts/ScriptsLoader.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/core/Material/Material.h"
#include "PulseEngine/core/Material/Texture.h"
#include "PulseEngine/core/Material/MaterialManager.h"
#include "PulseEngine/API/EntityAPI/EntityApi.h"
#include "PulseEngine/core/Meshes/RenderableMesh.h"
#include "PulseEngine/core/Meshes/StaticMesh.h"
#include "PulseEngine/core/Meshes/SkeletalMesh.h"

#include <assimp/Importer.hpp>      // Assimp::Importer
#include <assimp/scene.h>           // aiScene
#include <assimp/postprocess.h>     // postprocessing flags

#include <set>
using namespace PulseEngine::FileSystem;

Entity *GuidReader::GetEntityFromGuid(std::size_t guid)
{
    
    static int count = 0;
    std::string name = "Entity_" + std::to_string(count++);
    Entity* entity = new Entity(name, PulseEngine::Vector3(0.0f), nullptr, MaterialManager::loadMaterial("Materials/cube.mat"));

    nlohmann::json guidCollection;
    std::ifstream guidColFile(std::string(ASSET_PATH) +"Guid/guidCollectionEntities.puid");
    if(!guidColFile.is_open())
    {
        EDITOR_ERROR("Guid collection file for entities couldn't be open : " + std::string(ASSET_PATH) +"Guid/guidCollectionEntities.puid")
        delete entity;
        return nullptr;
    }
    
    guidColFile >> guidCollection;

    if(guidCollection.contains(std::to_string(guid)))
    {
        
        nlohmann::json entityData;
        std::ifstream entityFile(std::string(ASSET_PATH) + std::string(guidCollection[std::to_string(guid)]));
        if(!entityFile.is_open())
        {
            EDITOR_ERROR("Entity guid file couldn't be open : " + std::string(ASSET_PATH) + std::string(guidCollection[std::to_string(guid)]))
            delete entity;
            return nullptr;
        }
        
        entityFile >> entityData;
        return GetEntityFromJson(entityData, entity);
    }
    else
    {
        EDITOR_ERROR("Guid " + std::to_string(guid) + " not found in guid collection file for entities : " + std::string(ASSET_PATH) +"Guid/guidCollectionEntities.puid")
        delete entity;
        return nullptr;
    }
}

Entity *GuidReader::GetEntityFromJson(nlohmann::json_abi_v3_12_0::json &entityData, Entity *entity)
{
    if(entityData.contains("Guid"))
    {
        std::size_t entityGuid = 0;
        if (entityData["Guid"].is_string())
        {
            entityGuid = std::stoull(entityData["Guid"].get<std::string>());
        }
        else if (entityData["Guid"].is_number_unsigned())
        {
            entityGuid = entityData["Guid"].get<std::size_t>();
        }
        entity->SetGuid(entityGuid);
        EDITOR_LOG("Loading entity with GUID: " + std::to_string(entityGuid))
    }
    if (entityData.contains("Meshes"))
    {

        for (const auto &mesh : entityData["Meshes"])
        {

            std::size_t meshGuid = 0;
            try
            {
                if (mesh["Guid"].is_string())
                {
                    meshGuid = std::stoull(mesh["Guid"].get<std::string>());
                }
                else if (mesh["Guid"].is_number_unsigned())
                {
                    meshGuid = mesh.get<std::size_t>();
                }
                else
                {
                    EDITOR_ERROR("[GetEntityFromGuid] Invalid mesh GUID format.")
                    continue;
                }

                RenderableMesh* msh = GetMeshFromGuid(meshGuid);
                msh->transform.position.x = mesh["Position"][0].get<float>();
                msh->transform.position.y = mesh["Position"][1].get<float>();
                msh->transform.position.z = mesh["Position"][2].get<float>();
                            
                msh->transform.rotation.x = mesh["Rotation"][0].get<float>();
                msh->transform.rotation.y = mesh["Rotation"][1].get<float>();
                msh->transform.rotation.z = mesh["Rotation"][2].get<float>();
                            
                msh->transform.scale.x = mesh["Scale"][0].get<float>();
                msh->transform.scale.y = mesh["Scale"][1].get<float>();
                msh->transform.scale.z = mesh["Scale"][2].get<float>();
                msh->SetGuid(meshGuid);
                msh->SetName(mesh["Name"].get<std::string>());

                if (msh)
                {
                    EDITOR_LOG("Loaded mesh: " + msh->GetName() + " with GUID: " + std::to_string(meshGuid))
                    entity->AddMesh(msh);
                }
                else
                {
                    EDITOR_ERROR("Failed to load mesh with GUID: " + std::to_string(meshGuid))
                }
            }
            catch (const std::exception &e)
            {
                EDITOR_ERROR("[GetEntityFromGuid] Error parsing mesh guid: " << e.what())
            }
        }
    }
    EDITOR_LOG("Entity has " << entity->GetMeshes().size() << " meshes.")
    if (entityData.contains("Scripts"))
    {

        for (const auto &script : entityData["Scripts"])
        {

            IScript *scriptLoaded = ScriptsLoader::GetScriptFromCallName(script["Name"]);
            if (!scriptLoaded)
            {

                continue;
            }
            scriptLoaded->isEntityLinked = true;
            scriptLoaded->SetGUID(script["Guid"].get<std::size_t>());
            scriptLoaded->owner = new PulseEngine::EntityApi(entity);
            entity->AddScript(scriptLoaded);
        }
    }

    EDITOR_LOG("Entity has " << entity->GetScripts().size() << " scripts.")
    if (entityData.contains("Material"))
    {
        nlohmann::json_abi_v3_12_0::json jsonFile;
        Material* mat = nullptr;
        for(auto& material : GuidReader::GetAllAvailableFiles("guidCollectionMaterials.puid"))
        {
            if(material.first == entityData["Material"])
            {
                mat = MaterialManager::loadMaterial(material.second);
            }
        }

        entity->SetMaterial(mat);
    }

    EDITOR_LOG("Entity has material: " << (entity->GetMaterial() ? entity->GetMaterial()->GetName() : "None"))

    return entity;
}

Material* GuidReader::GetMaterialFromJson(nlohmann::json_abi_v3_12_0::json &materialData)
{
    std::string vertPath = std::string(ASSET_PATH) + "shaders/basic.vert";
    std::string fragPath = std::string(ASSET_PATH) + "shaders/basic.frag";
    
    Material* mat = new Material(materialData["name"], new Shader(vertPath, fragPath, PulseEngineGraphicsAPI));
    
    // mat->roughness = materialData["roughness"].get<float>();
    mat->specular = materialData["specular"].get<float>();
    
    auto colorArray = materialData["color"];
    mat->color = PulseEngine::Vector3(
        colorArray[0].get<float>(),
        colorArray[1].get<float>(),
        colorArray[2].get<float>()
    );

    for (auto it = materialData.begin(); it != materialData.end(); ++it)
    {
        const std::string& key = it.key();

        if (key.size() >= 4 && key.compare(0, 4, "txt_") == 0)
        {
            std::shared_ptr<Texture> txt = std::make_shared<Texture>(it.value().get<std::string>(), PulseEngineGraphicsAPI);
            mat->SetTexture(key.substr(4), txt);
        }
    } 

    return mat;
}


RenderableMesh* GuidReader::GetMeshFromGuid(std::size_t guid)
{
    std::string path = "";
    std::string meshPath = "";
    Assimp::Importer* importer = new Assimp::Importer();

    nlohmann::json guidCollection;
    std::ifstream guidColFile(std::string(ASSET_PATH) +"Guid/guidCollectionMeshes.puid");
    if(!guidColFile.is_open())
    {
        EDITOR_ERROR("Guid collection file for meshes couldn't be open : " + std::string(ASSET_PATH) +"Guid/guidCollectionMeshes.puid")
         return nullptr;
    }
    guidColFile >> guidCollection;
    if(guidCollection.contains(std::to_string(guid)))
    {
        path = guidCollection[std::to_string(guid)].get<std::string>();
        if (path.empty())
        {
            EDITOR_ERROR("Path for GUID " + std::to_string(guid) + " is empty.")
            return nullptr;
        }
        path = std::string(ASSET_PATH) + path;
        std::ifstream file(path);
        if(file.is_open())
        {
            nlohmann::json fileData;
            file >> fileData;
            if(fileData.contains("MeshPath"))
            {
                meshPath = fileData["MeshPath"].get<std::string>();
                if(meshPath.empty())
                {
                    EDITOR_ERROR("Mesh path for GUID " + std::to_string(guid) + " is empty.")
                    return nullptr;
                }
                meshPath = std::string(ASSET_PATH) + meshPath;
            }
            else
            {
                EDITOR_ERROR("MeshPath not found in JSON for GUID " + std::to_string(guid) + ".")
                return nullptr;
            }
        }
    }
    else
    {
        EDITOR_ERROR("Guid " + std::to_string(guid) + " not found in guid collection file for meshes : " + std::string(ASSET_PATH) +"Guid/guidCollectionMeshes.puid")
        return nullptr;
    }
    //onced shared system implemented, we will use importer->ReadFileFromMemory() instead
    // it will be needed to have a method in our shared system that sent back a vector of char from the .pak
    //the importer will read the object from the .pak like this :
    //std::vector<char> fileData = LoadFromPak("assets/model.fbx");
    //Assimp::Importer importer;
    //
    //const aiScene* scene = importer.ReadFileFromMemory(
    //    fileData.data(),                   // pointer to memory
    //    fileData.size(),                   // size
    //    aiProcess_Triangulate |
    //    aiProcess_FlipUVs |
    //    aiProcess_GenNormals |
    //    aiProcess_CalcTangentSpace,
    //    "fbx"                              // file extension/hint
    //);
    const aiScene* scene = importer->ReadFile(
        meshPath,
    aiProcess_Triangulate | 
    aiProcess_GenSmoothNormals |
    aiProcess_CalcTangentSpace | 
    aiProcess_JoinIdenticalVertices |
    aiProcess_ImproveCacheLocality |
    aiProcess_LimitBoneWeights |
    aiProcess_OptimizeMeshes |
    aiProcess_OptimizeGraph
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        EDITOR_ERROR("Erreur Assimp: " << importer->GetErrorString())
        return nullptr;
    }

    EDITOR_LOG("number of meshes " << scene->mNumMeshes);

    RenderableMesh* meshsObj = nullptr;

    if(scene->mNumAnimations > 0)
    {
        meshsObj = new SkeletalMesh(scene->mName.C_Str());
        SkeletalMesh* cst = dynamic_cast<SkeletalMesh*>(meshsObj);

        LoadSkeletonFromAssimp(cst, scene);

        // for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        // {
        //     const aiAnimation* anim = scene->mAnimations[i];
        //     AnimationClip clip = SkeletalMesh::LoadAnimationSimplified(anim);
        //     cst->animations.push_back(clip);
        // }

        LoadAnimationsFromAssimp(cst, scene);

        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            Mesh* msh = Mesh::LoadFromAssimp(scene->mMeshes[i], scene, cst); // OK: importer toujours vivant ici

            msh->importer = importer;
            msh->SetGuid(guid);
            msh->SetName(meshPath);
            meshsObj->AddMesh(msh);
        }


        cst->finalBoneMatrices.resize(cst->skeleton.size(), PulseEngine::Mat4(1.0f));
    }
    else
    {
        meshsObj = new StaticMesh(scene->mName.C_Str());
        for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        {
            Mesh* msh = Mesh::LoadFromAssimp(scene->mMeshes[i], scene, nullptr); // OK: importer toujours vivant ici

            msh->importer = importer;
            msh->SetGuid(guid);
            msh->SetName(meshPath);
            meshsObj->AddMesh(msh);
        }
    }

    return meshsObj;
}

std::size_t GuidReader::InsertIntoCollection(const std::string &filePath)
{
    std::string path = filePath;
    std::replace(path.begin(), path.end(), '\\', '/'); // Normalize path for consistency
    const std::string prefix = "PulseEngineEditor/";
    if (path.compare(0, prefix.size(), prefix) == 0) {
        path = path.substr(prefix.size());
    }
    std::string collectionType = FileManager::GetCollectionByExtension(path);
    std::size_t guid = GenerateGUIDFromPath(path);


    std::string collectionPath = GUID_COLLECTION_PATH + collectionType;
    EDITOR_LOG(collectionPath)    

    // Load existing collection
    nlohmann::json jsonData;
EDITOR_LOG("Working directory: " << std::filesystem::current_path())
EDITOR_LOG("Full collection path: " << collectionPath)
    std::ifstream inFile(collectionPath);
    if (inFile.is_open()) {
        // If file exists, read the existing JSON data
        inFile >> jsonData;
        inFile.close();
    } else {
        // If file doesn't exist, initialize an empty JSON object
        EDITOR_ERROR("File not found, creating new collection file.")
        jsonData = nlohmann::json::object();  // Initialize an empty JSON object
    }

    // Insert or update entry
    jsonData[std::to_string(guid)] = path;

    // Rewrite the entire file (create or update the file)
    std::ofstream outFile(collectionPath);
    if (outFile.is_open()) {
        outFile << jsonData.dump(4); // 4 = pretty print with indentation
        outFile.close();
    } else {
        EDITOR_ERROR("Failed to open GUID collection file for writing.")
    }

    return guid;
}

std::vector<std::pair<std::string, std::string>> GuidReader::GetAllAvailableFiles(const std::string &guidFile)
{
    std::string collectionType = FileManager::GetCollectionByExtension(guidFile);
    std::string collectionPath = GUID_COLLECTION_PATH + guidFile;
    nlohmann::json jsonData;
    std::ifstream inFile(collectionPath);

    if (inFile.is_open()) {
        inFile >> jsonData;
        inFile.close();
        
        std::vector<std::pair<std::string, std::string>> files;
        for (const auto& item : jsonData.items()) {
            files.push_back(std::make_pair(item.key(), item.value().get<std::string>()));
        }
        return files;
    } else {
        EDITOR_ERROR("Failed to open GUID collection file.")
    }

    return std::vector<std::pair<std::string, std::string>>();
}

void GuidReader::LoadSkeletonFromAssimp(SkeletalMesh* skel, const aiScene* scene)
{

    // Step 1: collect all bones from all meshes
    int boneCounter = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int b = 0; b < mesh->mNumBones; ++b)
        {
            aiBone* aiBonePtr = mesh->mBones[b];
            std::string boneName = aiBonePtr->mName.C_Str();
            if (skel->boneNameToIndex.find(boneName) == skel->boneNameToIndex.end())
            {
                Bone bone;
                bone.name = boneName;
                bone.index = boneCounter++;
                bone.offsetMatrix = SkeletalMesh::ConvertAiMatrix(aiBonePtr->mOffsetMatrix);
                skel->skeleton.push_back(bone);
                skel->boneNameToIndex[boneName] = bone.index;
            }
        }
    }

    // Step 2: build hierarchy from scene root node
    std::function<void(aiNode*, int)> recurse = [&](aiNode* node, int parentIndex)
    {
        std::string nodeName = node->mName.C_Str();
        auto it = skel->boneNameToIndex.find(nodeName);
        int currentIndex = (it != skel->boneNameToIndex.end()) ? it->second : -1;

        if (currentIndex != -1)
        {
            skel->skeleton[currentIndex].parentIndex = parentIndex;
            skel->skeleton[currentIndex].localTransform = SkeletalMesh::ConvertAiMatrix(node->mTransformation);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            recurse(node->mChildren[i], currentIndex != -1 ? currentIndex : parentIndex);
    };

    recurse(scene->mRootNode, -1);
}

void GuidReader::LoadAnimationsFromAssimp(SkeletalMesh* skel, const aiScene* scene)
{
    for (unsigned int a = 0; a < scene->mNumAnimations; ++a)
    {
        aiAnimation* anim = scene->mAnimations[a];
        AnimationClip clip;
        clip.name = anim->mName.C_Str();
        clip.duration = anim->mDuration;
        clip.tickPerSeconds = anim->mTicksPerSecond != 0.0 ? anim->mTicksPerSecond : 30.0;

        // Step 1: Collect all unique timestamps
        std::set<double> allTimes;
        for (unsigned int c = 0; c < anim->mNumChannels; ++c)
        {
            aiNodeAnim* channel = anim->mChannels[c];

            for (unsigned int i = 0; i < channel->mNumPositionKeys; ++i)
                allTimes.insert(channel->mPositionKeys[i].mTime);
            for (unsigned int i = 0; i < channel->mNumRotationKeys; ++i)
                allTimes.insert(channel->mRotationKeys[i].mTime);
            for (unsigned int i = 0; i < channel->mNumScalingKeys; ++i)
                allTimes.insert(channel->mScalingKeys[i].mTime);
        }

        // Step 2: Initialize last known transform for each bone
        std::unordered_map<std::string, TransformAnimation> lastTransform;
        for (const Bone& bone : skel->skeleton)
        {
            lastTransform[bone.name] = {
                PulseEngine::Vector3(0.0f),          // position fallback
                PulseEngine::Vector3(1.0f),          // scale fallback
                PulseEngine::Vector4(0,0,0,1)        // rotation fallback (identity quaternion)
            };
        }

        // Step 3: Build keyframes for each timestamp
        for (double t : allTimes)
        {
            KeyFrame frame;
            frame.time = t;

            for (unsigned int c = 0; c < anim->mNumChannels; ++c)
            {
                aiNodeAnim* channel = anim->mChannels[c];
                std::string boneName = channel->mNodeName.C_Str();
                TransformAnimation& transform = lastTransform[boneName];

                // Update position if there’s a key at this time
                for (unsigned int i = 0; i < channel->mNumPositionKeys; ++i)
                {
                    if (channel->mPositionKeys[i].mTime == t)
                    {
                        transform.position = {
                            channel->mPositionKeys[i].mValue.x,
                            channel->mPositionKeys[i].mValue.y,
                            channel->mPositionKeys[i].mValue.z
                        };
                        break;
                    }
                }

                // Update rotation if there’s a key at this time
                for (unsigned int i = 0; i < channel->mNumRotationKeys; ++i)
                {
                    if (channel->mRotationKeys[i].mTime == t)
                    {
                        transform.rotation = {
                            channel->mRotationKeys[i].mValue.x,
                            channel->mRotationKeys[i].mValue.y,
                            channel->mRotationKeys[i].mValue.z,
                            channel->mRotationKeys[i].mValue.w
                        };
                        break;
                    }
                }

                // Update scale if there’s a key at this time
                for (unsigned int i = 0; i < channel->mNumScalingKeys; ++i)
                {
                    if (channel->mScalingKeys[i].mTime == t)
                    {
                        transform.scale = {
                            channel->mScalingKeys[i].mValue.x,
                            channel->mScalingKeys[i].mValue.y,
                            channel->mScalingKeys[i].mValue.z
                        };
                        break;
                    }
                }

                frame.boneTransforms[boneName] = transform;
            }

            // Step 4: Make sure all bones have a transform
            for (const Bone& bone : skel->skeleton)
            {
                if (frame.boneTransforms.find(bone.name) == frame.boneTransforms.end())
                    frame.boneTransforms[bone.name] = lastTransform[bone.name];
            }

            clip.keyframes.push_back(frame);
        }

        skel->animations.push_back(clip);
    }
}



