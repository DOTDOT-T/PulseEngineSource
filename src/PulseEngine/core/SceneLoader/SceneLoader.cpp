#include "SceneLoader.h"
#include "PulseEngine/core/GUID/GuidReader.h"
#include "PulseEngine/core/Entity/Entity.h"
#include "PulseEngine/core/PulseEngineBackend.h"
#include "PulseEngine/CustomScripts/IScripts.h"
#include "PulseEngine/CustomScripts/ScriptsLoader.h"
#include "PulseEngine/core/Material/MaterialManager.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"
#include "PulseEngine/core/Meshes/primitive/Primitive.h"
#include "PulseEngine/core/Meshes/primitive/Primitive.h"
#include "PulseEngine/core/Lights/Lights.h"
#include "PulseEngine/core/Lights/DirectionalLight/DirectionalLight.h"
#include "PulseEngine/core/Lights/PointLight/PointLight.h"
#include "PulseEngine/API/EntityAPI/EntityApi.h"
#include "PulseEngine/core/FileManager/Archive/DiskArchive.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "PulseEngine/core/SceneManager/SceneManager.h"
#include "PulseEngine/core/Gamemode/Gamemode.h"

#include <iostream>
#include <assimp/Importer.hpp>      // Assimp::Importer
#include <assimp/scene.h>           // aiScene
#include <assimp/postprocess.h>     // postprocessing flags

#pragma region SceneLoader
void SceneLoader::LoadScene(const std::string &mapName, PulseEngineBackend* backend)
{
    PROFILE_TIMER_FUNCTION;
    std::ifstream scene(std::string(ASSET_PATH) + mapName);
    if (!scene.is_open())
    {
        EDITOR_WARN("Couldn't open map " << std::string(ASSET_PATH) + mapName)
        return;
    }
    backend->ClearScene();
    SceneManager::GetInstance()->CleanHierarchyFrom(SceneManager::GetInstance()->GetRoot());

    // nlohmann::json sceneData;
    // scene >> sceneData;

    
    DiskArchive dar(mapName, Archive::Mode::Loading);
    int entityCount;
    std::string receivedMapName;
    int guid;
    dar.Serialize("sceneName", receivedMapName);
    dar.Serialize("guid", guid);
    dar.Serialize("entitiesCount", entityCount);
    for(unsigned int i = 0; i < entityCount; i++)
    {
        std::string typeName;
        std::uint64_t guid;
        std::uint64_t muid;
        dar.Serialize("typeName", typeName);
        dar.Serialize("guid", guid);
        dar.Serialize("muid", muid);
        
        if(TypeRegistry::IsRegistered(typeName))
        {            
            Entity* po = GuidReader::GetEntityFromGuid(static_cast<std::size_t>(guid));
            if(!po)
            {
                EDITOR_ERROR("entity with guid " << guid << "couldn't be loaded correctly")
                continue;
            }
            po->SetMuid(muid);
            po->Serialize(dar);


            backend->entities.push_back(po);
            SceneManager::GetInstance()->InsertEntity(po);
            EDITOR_LOG("Spawning " << po->ToString() << " transform -> " << po->transform.ToString())
        }
    }

    int lightCount = (int)backend->lights.size();
    dar.Serialize("lightCount", lightCount);
    for(unsigned int i = 0; i < lightCount; i++)
    {
        std::string typeName;
        dar.Serialize("typeName", typeName);
        LightData* light = TypeRegistry::CreateInstance<LightData>(typeName);
        light->Serialize(dar);

        backend->lights.push_back(light);
        SceneManager::GetInstance()->InsertEntity(light);
    }

    PulseEngineInstance->actualMapPath = mapName;
     size_t lastSlash = mapName.find_last_of("/\\");
     if (lastSlash != std::string::npos && lastSlash + 1 < mapName.size()) {
         PulseEngineInstance->actualMapName = mapName.substr(lastSlash + 1);
     } else {
         PulseEngineInstance->actualMapName = mapName;
     }

    PulseEngineInstance->SetWindowName(PulseEngineInstance->actualMapName);

    DiskArchive dar("enginegm.gamemode", Archive::Mode::Loading);
    PulseEngineInstance->GetGamemode()->Serialize(gmdar);

    // backend->SetWindowName(sceneData["sceneName"]);
    // for (const auto& entityData : sceneData["entities"])
    // {        
    //     EDITOR_LOG("creating a new entity")
    //     Entity* entity = LoadEntityBaseData(entityData);
    //     EDITOR_LOG("loaded entity base data ")
    //     if(entity)
    //     {
    //         for(auto& script : entityData["Scripts"])
    //         {
    //             IScript* existingScript = nullptr;
    //             if(script["isEntityLinked"].get<bool>())
    //             {
    //                 for(auto& entityScript : entity->GetScripts())
    //                 {
    //                     if(entityScript->GetGUID() == script["guid"].get<std::size_t>())
    //                     {
    //                         existingScript = entityScript;
    //                         break;
    //                     }
    //                 }
    //             }
    //             LoadEntityScript(script, entity, existingScript);              
    //         }
    //         EDITOR_LOG("adding the entity to the backend")
    //         EDITOR_LOG("entity guid: " << entity->GetGuid())
    //         EDITOR_LOG("entity muid: " << entity->GetMuid())
    //         backend->entities.push_back(entity);
    //     }
    //     else
    //     {
    //         EDITOR_LOG("entity is not valid !")
    //     }
    // }
    // for (const auto& lightData : sceneData["lights"])
    // {
    //     LightData* light = nullptr;
    //     std::string type = lightData["type"].get<std::string>();
    //     if (type == "DirectionalLight")
    //     {
    //         light = new DirectionalLight(lightData["nearPlane"].get<float>(),
    //                                      lightData["farPlane"].get<float>(),
    //                                      PulseEngine::Vector3(lightData["target"][0].get<float>(), 
    //                                                           lightData["target"][1].get<float>(),
    //                                                           lightData["target"][2].get<float>()),
    //                                      PulseEngine::Vector3(lightData["position"][0].get<float>(),
    //                                                           lightData["position"][1].get<float>(),
    //                                                           lightData["position"][2].get<float>()),
    //                                      PulseEngine::Color(lightData["color"][0].get<float>(),
    //                                                           lightData["color"][1].get<float>(),
    //                                                           lightData["color"][2].get<float>()),
    //                                     lightData["intensity"].get<float>(), lightData["attenuation"].get<float>());
    //     }
    //     else if (type == "PointLight")
    //     {
    //         light = new PointLight(PulseEngine::Vector3(lightData["position"][0].get<float>(),
    //                                                     lightData["position"][1].get<float>(),
    //                                                     lightData["position"][2].get<float>()),
    //                                 PulseEngine::Color(lightData["color"][0].get<float>(),
    //                                                     lightData["color"][1].get<float>(),
    //                                                     lightData["color"][2].get<float>()),
    //                                 lightData["intensity"].get<float>(), lightData["attenuation"].get<float>(), lightData["farPlane"].get<float>());
    //     }
    //     if (light)
    //     {
    //         light->SetName(lightData["name"].get<std::string>());
    //         light->SetMuid(lightData["muid"].get<std::size_t>());
    //         backend->lights.push_back(light);
    //         EDITOR_LOG("Light " << light->GetName() << " loaded.")
    //     }
    // }
    // EDITOR_LOG("Scene " << mapName << " loaded successfully.")
    // // Set actualMapName to the substring after the last "/"

}

void SceneLoader::LoadEntityScript(const nlohmann::json_abi_v3_12_0::json &script, Entity *entity, IScript* existingScript)
{
    std::string scriptName = script["name"].get<std::string>();
    std::vector<ExposedVariable *> exposedVariables;
    for (const auto &var : script["exposedVariables"])
    {
        ExtractExposedVariable(var, exposedVariables);
    }
    IScript *scriptInstance = existingScript ? existingScript : ScriptsLoader::GetScriptFromCallName(scriptName);
    if (scriptInstance)
    {
        if(!existingScript) scriptInstance->isEntityLinked = false;
        if(!existingScript) scriptInstance->owner = new PulseEngine::EntityApi(entity);
        for (const auto &var : exposedVariables)
        {
            if(!existingScript) scriptInstance->AddExposedVariable(*var);
            void *dst = scriptInstance->GetVariableByName(var->name);
            if (dst)
            {
                switch (var->type)
                {
                case ExposedVariable::Type::FLOAT:
                    *reinterpret_cast<float *>(dst) = *reinterpret_cast<float *>(var->ptr);
                    break;
                case ExposedVariable::Type::INT:
                    *reinterpret_cast<int *>(dst) = *reinterpret_cast<int *>(var->ptr);
                    break;
                case ExposedVariable::Type::BOOL:
                    *reinterpret_cast<bool *>(dst) = *reinterpret_cast<bool *>(var->ptr);
                    break;
                case ExposedVariable::Type::STRING:
                    *reinterpret_cast<std::string *>(dst) = *reinterpret_cast<std::string *>(var->ptr);
                    break;
                }
            }

            EDITOR_LOG("Exposed variable name: " + var->name)
        }
        if(!existingScript) {
            entity->AddScript(scriptInstance);
            scriptInstance->OnStart();
        }
        EDITOR_LOG("Script " << scriptName << " loaded.")
    }
    else
    {
        EDITOR_LOG("Script " << scriptName << " not found.")
    }
}

void SceneLoader::ExtractExposedVariable(const nlohmann::json_abi_v3_12_0::json &var, std::vector<ExposedVariable *> &exposedVariables)
{
    ExposedVariable::Type type = static_cast<ExposedVariable::Type>(var["type"].get<int>());
    std::string name = var["name"].get<std::string>();
    void *ptr = nullptr;

    switch (type)
    {
    case ExposedVariable::Type::INT:
        ptr = new int(var["value"].get<int>());
        break;
    case ExposedVariable::Type::FLOAT:
        ptr = new float(var["value"].get<float>());
        break;
    case ExposedVariable::Type::BOOL:
        ptr = new bool(var["value"].get<bool>());
        break;
    case ExposedVariable::Type::STRING:
        ptr = new std::string(var["value"].get<std::string>());
        break;
    }

    exposedVariables.push_back(new ExposedVariable(name, type, ptr));
}

Entity* SceneLoader::LoadEntityBaseData(const nlohmann::json_abi_v3_12_0::json &entityData)
{
    PulseEngine::Vector3 position;
    PulseEngine::Vector3 rotation;
    PulseEngine::Vector3 scale;
    position.x = entityData["position"][0].get<float>();
    position.y = entityData["position"][1].get<float>();
    position.z = entityData["position"][2].get<float>();

    rotation.x = entityData["rotation"][0].get<float>();
    rotation.y = entityData["rotation"][1].get<float>();
    rotation.z = entityData["rotation"][2].get<float>();

    scale.x = entityData["scale"][0].get<float>();
    scale.y = entityData["scale"][1].get<float>();
    scale.z = entityData["scale"][2].get<float>();
    std::string name = entityData["name"].get<std::string>();

    std::size_t guid = entityData["guid"].get<std::size_t>();
    std::size_t muid = entityData["muid"].get<std::size_t>();
    nlohmann::json entityDocData;

    EDITOR_LOG("Loading entity base data:")
    EDITOR_LOG("  Name: " << name)
    EDITOR_LOG("  GUID: " << guid)
    EDITOR_LOG("  MUID: " << muid)
    EDITOR_LOG("  Position: (" << position.x << ", " << position.y << ", " << position.z << ")")
    EDITOR_LOG("  Rotation: (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ")")
    EDITOR_LOG("  Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")")

    Entity* entity = GuidReader::GetEntityFromGuid(guid);
    EDITOR_LOG("Entity retrieved from GUID: " << (entity ? "Valid" : "Invalid"))
    if (!entity)
    {
        EDITOR_LOG("entity not valid")
        return nullptr;
    };
    entity->SetGuid(guid);
    entity->SetMuid(muid);
    entity->SetPosition(position);
    entity->SetRotation(rotation);
    entity->SetScale(scale);
    entity->SetName(name);
    // entity->SetMaterial(MaterialManager::loadMaterial("Materials/cube.mat"));
    EDITOR_LOG("Entity base data loaded and properties set.")
    return entity;
}

std::vector<std::string> SceneLoader::GetSceneFiles(const std::string &directory)
{
    std::vector<std::string> sceneFiles;
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".pmap")
        {
            // Normalize path separators and remove everything before "/Scenes/"
            std::string path = entry.path().generic_string(); // uses '/' as separator
            std::size_t pos = path.find("/Scenes/");
            if (pos != std::string::npos)
            {
                sceneFiles.push_back(path.substr(pos));
            }
            else
            {
                // fallback: just use the filename
                sceneFiles.push_back("/Scenes/" + entry.path().filename().string());
            }
        }
    }
    return sceneFiles;
}

const aiScene* SceneLoader::LoadSceneFromAssimp(std::string path)
{    
    PROFILE_TIMER_FUNCTION;
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals|
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        EDITOR_ERROR("Erreur Assimp: " << importer.GetErrorString())
        return nullptr;
    }

    EDITOR_LOG("Modèle chargé avec succès : " << path)
    EDITOR_LOG("Nombre de meshes : " << scene->mNumMeshes)
    EDITOR_LOG("Nombre de matériaux : " << scene->mNumMaterials)
    EDITOR_LOG("Nombre de textures : " << scene->mNumTextures)

    // Affichage du premier mesh pour exemple
    if (scene->mNumMeshes > 0)
    {
        aiMesh* mesh = scene->mMeshes[0];
        EDITOR_LOG("Premier mesh contient " << mesh->mNumVertices << " sommets.")
    }
    return scene;
}

#pragma endregion

#pragma region SaveScene
void SceneLoader::SaveSceneToFile(const std::string &mapName, const std::string& mapPath, PulseEngineBackend *backend)
{
    //~second implementation with pulseobject for test

    DiskArchive dar(mapPath, Archive::Mode::Saving);
    int entitiesSize = (int)backend->entities.size();
    std::string map = mapName;
    int guid;
    dar.Serialize("sceneName", map);
    dar.Serialize("guid", guid);
    dar.Serialize("entitiesCount", entitiesSize);
    for(Entity* en : backend->entities)
    {
        std::string typeName(en->GetTypeName());
        std::uint64_t guid = en->GetGuid();
        std::uint64_t muid = en->GetMuid();
        dar.Serialize("typeName", typeName);
        dar.Serialize("guid", guid);
        dar.Serialize("muid", muid);
        en->Serialize(dar);
    }

    int lightCount = (int)backend->lights.size();
    dar.Serialize("lightCount", lightCount);
    for(LightData* light : backend->lights)
    {
        std::string typeName(light->GetTypeName());
        dar.Serialize("typeName", typeName);
        light->Serialize(dar);
    }

    dar.Finalize();


    //~first implementation of saving without pulseobject 

    // nlohmann::json sceneData;

    // // Set the scene name
    // sceneData["sceneName"] = mapName;

    // // Save entities
    // for (const auto& entity : backend->entities)
    // {
    //     SaveEntities(entity, sceneData);
    // }

    // for (const auto& light : backend->lights)
    // {
    //     nlohmann::json lightData;
    //     if(DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(light))
    //     {
    //         lightData["type"] = "DirectionalLight";
    //         lightData["position"] = {dirLight->GetPosition().x, dirLight->GetPosition().y, dirLight->GetPosition().z};
    //         lightData["color"] = {dirLight->color.r, dirLight->color.g, dirLight->color.b};
    //         lightData["intensity"] = dirLight->intensity;
    //         lightData["attenuation"] = dirLight->attenuation;
    //         lightData["farPlane"] = dirLight->farPlane;
    //         lightData["nearPlane"] = dirLight->nearPlane;
    //         lightData["target"] = {dirLight->target.x, dirLight->target.y, dirLight->target.z};
    //         lightData["castsShadow"] = dirLight->castsShadow;
    //     }
    //     else if(PointLight* pointLight = dynamic_cast<PointLight*>(light))
    //     {
    //         lightData["type"] = "PointLight";
    //         lightData["position"] = {pointLight->GetPosition().x, pointLight->GetPosition().y, pointLight->GetPosition().z};
    //         lightData["color"] = {pointLight->color.r, pointLight->color.g, pointLight->color.b};
    //         lightData["intensity"] = pointLight->intensity;
    //         lightData["attenuation"] = pointLight->attenuation;
    //         lightData["farPlane"] = pointLight->farPlane;
    //         lightData["castsShadow"] = pointLight->castsShadow;
    //     }
    //     else
    //     {
    //         EDITOR_LOG("Unknown light type, skipping.")
    //         continue;
    //     }
    //     lightData["name"] = light->GetName();
    //     lightData["muid"] = light->GetMuid();
    //     sceneData["lights"].push_back(lightData);
    // }

    // // Write to file
    // std::ofstream sceneFile(std::string(ASSET_PATH) + mapPath);
    // if (!sceneFile.is_open())
    // {
    //     EDITOR_LOG("Couldn't open file to save map " << mapPath)
    //     return;
    // }

    // sceneFile << sceneData.dump(4); // Pretty print with 4 spaces
    // sceneFile.close();
}

void SceneLoader::SaveEntities(Entity *const &entity, nlohmann::json_abi_v3_12_0::json &sceneData)
{
    nlohmann::json entityData;

    SaveBaseDataEntity(entity, entityData);

    nlohmann::json scriptsData = nlohmann::json::array();
    for (const auto &script : entity->GetScripts())
    {
        nlohmann::json scriptData;
        scriptData["name"] = script->GetName();
        scriptData["exposedVariables"] = nlohmann::json::array();
        for (const auto &var : script->GetExposedVariables())
        {
            nlohmann::json varDt;
            varDt["name"] = var.name;
            varDt["type"] = var.type;
            switch (var.type)
            {
            case ExposedVariable::Type::INT:
                varDt["value"] = *reinterpret_cast<int *>(var.ptr);
                break;
            case ExposedVariable::Type::FLOAT:
                varDt["value"] = *reinterpret_cast<float *>(var.ptr);
                break;
            case ExposedVariable::Type::BOOL:
                varDt["value"] = *reinterpret_cast<bool *>(var.ptr);
                break;
            case ExposedVariable::Type::STRING:
                varDt["value"] = *reinterpret_cast<std::string *>(var.ptr);
                break;
            }
            scriptData["exposedVariables"].push_back(varDt);
            scriptData["guid"] = script->GetGUID();
            scriptData["isEntityLinked"] = script->isEntityLinked;
        }

        // Add the script name to the JSON array
        scriptsData.push_back(scriptData);
    }

    // Add the scripts to the entity data
    entityData["Scripts"] = scriptsData;

    sceneData["entities"].push_back(entityData);
}

void SceneLoader::SaveBaseDataEntity(Entity *const &entity, nlohmann::json_abi_v3_12_0::json &entityData)
{
    // Save position
    PulseEngine::Vector3 position = entity->GetPosition();
    entityData["position"] = {position.x, position.y, position.z};

    // Save rotation
    PulseEngine::Vector3 rotation = entity->GetRotation();
    entityData["rotation"] = {rotation.x, rotation.y, rotation.z};

    PulseEngine::Vector3 scale = entity->GetScale();
    entityData["scale"] = {scale.x, scale.y, scale.z};
    // Save name
    entityData["name"] = entity->GetName();

    // Save GUID
    entityData["guid"] = entity->GetGuid();
    entityData["muid"] = entity->GetMuid();
}

#pragma endregion