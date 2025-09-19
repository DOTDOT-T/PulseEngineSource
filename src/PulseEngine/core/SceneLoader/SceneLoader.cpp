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

#include <iostream>
#include <assimp/Importer.hpp>      // Assimp::Importer
#include <assimp/scene.h>           // aiScene
#include <assimp/postprocess.h>     // postprocessing flags

#pragma region SceneLoader
void SceneLoader::LoadScene(const std::string &mapName, PulseEngineBackend* backend)
{
    std::ifstream scene(std::string(ASSET_PATH) + mapName);
    if (!scene.is_open())
    {
        std::cout << "Couldn't open map " << std::string(ASSET_PATH) + mapName << std::endl;
        return;
    }
    backend->ClearScene();

    nlohmann::json sceneData;
    scene >> sceneData;

    backend->SetWindowName(sceneData["sceneName"]);

    for (const auto& entityData : sceneData["entities"])
    {        

        std::cout << "creating a new entity" << std::endl;

        Entity* entity = LoadEntityBaseData(entityData);

        std::cout << "loaded entity base data " << std::endl;

        if(entity)
        {
            for(auto& script : entityData["Scripts"])
            {
                IScript* existingScript = nullptr;
                if(script["isEntityLinked"].get<bool>())
                {
                    for(auto& entityScript : entity->GetScripts())
                    {
                        if(entityScript->GetGUID() == script["guid"].get<std::size_t>())
                        {
                            existingScript = entityScript;
                            break;
                        }
                    }
                }
                LoadEntityScript(script, entity, existingScript);
                
            }
            std::cout << "adding the entity to the backend" << std::endl;
            std::cout << "entity guid: " << entity->GetGuid() << std::endl;
            std::cout << "entity muid: " << entity->GetMuid() << std::endl;
            backend->entities.push_back(entity);
        }
        else
        {
            std::cout << "entity is not valid !" << std::endl;
        }
    }

    for (const auto& lightData : sceneData["lights"])
    {
        LightData* light = nullptr;
        std::string type = lightData["type"].get<std::string>();
        if (type == "DirectionalLight")
        {
            light = new DirectionalLight(lightData["nearPlane"].get<float>(),
                                         lightData["farPlane"].get<float>(),
                                         PulseEngine::Vector3(lightData["target"][0].get<float>(), 
                                                              lightData["target"][1].get<float>(),
                                                              lightData["target"][2].get<float>()),
                                         PulseEngine::Vector3(lightData["position"][0].get<float>(),
                                                              lightData["position"][1].get<float>(),
                                                              lightData["position"][2].get<float>()),
                                         PulseEngine::Color(lightData["color"][0].get<float>(),
                                                              lightData["color"][1].get<float>(),
                                                              lightData["color"][2].get<float>()),
                                        lightData["intensity"].get<float>(), lightData["attenuation"].get<float>());
        }
        else if (type == "PointLight")
        {
            light = new PointLight(PulseEngine::Vector3(lightData["position"][0].get<float>(),
                                                        lightData["position"][1].get<float>(),
                                                        lightData["position"][2].get<float>()),
                                    PulseEngine::Color(lightData["color"][0].get<float>(),
                                                        lightData["color"][1].get<float>(),
                                                        lightData["color"][2].get<float>()),
                                    lightData["intensity"].get<float>(), lightData["attenuation"].get<float>(), lightData["farPlane"].get<float>());
        }

        if (light)
        {
            light->SetName(lightData["name"].get<std::string>());
            light->SetMuid(lightData["muid"].get<std::size_t>());
            backend->lights.push_back(light);
            std::cout << "Light " << light->GetName() << " loaded." << std::endl;
        }
    }

    std::cout << "Scene " << mapName << " loaded successfully." << std::endl;
    PulseEngineInstance->actualMapPath = mapName;
    // Set actualMapName to the substring after the last "/"
    size_t lastSlash = mapName.find_last_of("/\\");
    if (lastSlash != std::string::npos && lastSlash + 1 < mapName.size()) {
        PulseEngineInstance->actualMapName = mapName.substr(lastSlash + 1);
    } else {
        PulseEngineInstance->actualMapName = mapName;
    }

    PulseEngineInstance->SetWindowName(PulseEngineInstance->actualMapName);
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

            std::cout << "Exposed variable name: " + var->name << std::endl;
        }
        if(!existingScript) {
            entity->AddScript(scriptInstance);
            scriptInstance->OnStart();
        }
        std::cout << "Script " << scriptName << " loaded." << std::endl;
    }
    else
    {
        std::cout << "Script " << scriptName << " not found." << std::endl;
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

    std::cout << "Loading entity base data:" << std::endl;
    std::cout << "  Name: " << name << std::endl;
    std::cout << "  GUID: " << guid << std::endl;
    std::cout << "  MUID: " << muid << std::endl;
    std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    std::cout << "  Rotation: (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ")" << std::endl;
    std::cout << "  Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")" << std::endl;

    Entity* entity = GuidReader::GetEntityFromGuid(guid);
    std::cout << "Entity retrieved from GUID: " << (entity ? "Valid" : "Invalid") << std::endl;
    if (!entity)
    {
        std::cout << "entity not valid" << std::endl;
        return nullptr;
    };
    entity->SetGuid(guid);
    entity->SetMuid(muid);
    entity->SetPosition(position);
    entity->SetRotation(rotation);
    entity->SetScale(scale);
    entity->SetName(name);
    // entity->SetMaterial(MaterialManager::loadMaterial("Materials/cube.mat"));
    std::cout << "Entity base data loaded and properties set." << std::endl;
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
        std::cerr << "Erreur Assimp: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::cout << "Modèle chargé avec succès : " << path << std::endl;
    std::cout << "Nombre de meshes : " << scene->mNumMeshes << std::endl;
    std::cout << "Nombre de matériaux : " << scene->mNumMaterials << std::endl;
    std::cout << "Nombre de textures : " << scene->mNumTextures << std::endl;

    // Affichage du premier mesh pour exemple
    if (scene->mNumMeshes > 0)
    {
        aiMesh* mesh = scene->mMeshes[0];
        std::cout << "Premier mesh contient " << mesh->mNumVertices << " sommets." << std::endl;
    }
    return scene;
}

#pragma endregion

#pragma region SaveScene
void SceneLoader::SaveSceneToFile(const std::string &mapName, const std::string& mapPath, PulseEngineBackend *backend)
{
    nlohmann::json sceneData;

    // Set the scene name
    sceneData["sceneName"] = mapName;

    // Save entities
    for (const auto& entity : backend->entities)
    {
        SaveEntities(entity, sceneData);
    }

    for (const auto& light : backend->lights)
    {
        nlohmann::json lightData;
        if(DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(light))
        {
            lightData["type"] = "DirectionalLight";
            lightData["position"] = {dirLight->GetPosition().x, dirLight->GetPosition().y, dirLight->GetPosition().z};
            lightData["color"] = {dirLight->color.r, dirLight->color.g, dirLight->color.b};
            lightData["intensity"] = dirLight->intensity;
            lightData["attenuation"] = dirLight->attenuation;
            lightData["farPlane"] = dirLight->farPlane;
            lightData["nearPlane"] = dirLight->nearPlane;
            lightData["target"] = {dirLight->target.x, dirLight->target.y, dirLight->target.z};
            lightData["castsShadow"] = dirLight->castsShadow;
        }
        else if(PointLight* pointLight = dynamic_cast<PointLight*>(light))
        {
            lightData["type"] = "PointLight";
            lightData["position"] = {pointLight->GetPosition().x, pointLight->GetPosition().y, pointLight->GetPosition().z};
            lightData["color"] = {pointLight->color.r, pointLight->color.g, pointLight->color.b};
            lightData["intensity"] = pointLight->intensity;
            lightData["attenuation"] = pointLight->attenuation;
            lightData["farPlane"] = pointLight->farPlane;
            lightData["castsShadow"] = pointLight->castsShadow;
        }
        else
        {
            std::cout << "Unknown light type, skipping." << std::endl;
            continue;
        }
        lightData["name"] = light->GetName();
        lightData["muid"] = light->GetMuid();
        sceneData["lights"].push_back(lightData);
    }

    // Write to file
    std::ofstream sceneFile(std::string(ASSET_PATH) + mapPath);
    if (!sceneFile.is_open())
    {
        std::cout << "Couldn't open file to save map " << mapPath << std::endl;
        return;
    }

    sceneFile << sceneData.dump(4); // Pretty print with 4 spaces
    sceneFile.close();
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