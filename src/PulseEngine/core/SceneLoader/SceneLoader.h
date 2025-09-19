/**
 * @file SceneLoader.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Helper class to load and save scene from/to json files readable by the engine.
 * @note need to transform every entity/light/mesh etc loader into a factory pattern to avoid code duplication and mess.
 * @version 0.1
 * @date 2025-08-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "Common/common.h"
#include "Common/dllExport.h"
#include "json.hpp"

class PulseEngineBackend;

struct ExposedVariable;
struct aiScene;
class IScript;

class PULSE_ENGINE_DLL_API SceneLoader
{
    public:
        ///====== SceneLoader ======
        /// This function is responsible of the loading of the scene, for the editor, and in game.
        /// The scene is loaded from a json file, with entities, and scripts attach to them.
        /// The scripts saved will have all the exposed variables saved, from the value set in the editor.
        static void LoadScene(const std::string &mapName, PulseEngineBackend *backend);
        static std::vector<std::string> GetSceneFiles(const std::string& directory);

        static const aiScene* LoadSceneFromAssimp(std::string path);

        ///====== SaveScene ======
        /// This function is responsible of the saving of the scene, for the editor.
        /// The scene is saved to a json file, with entities, and scripts attach to them.
        /// The scripts saved will have all the exposed variables saved, from the value set in the editor.
        static void SaveSceneToFile(const std::string &mapName, const std::string& mapPath, PulseEngineBackend *backend);

        static void SaveEntities(Entity *const &entity, nlohmann::json_abi_v3_12_0::json &sceneData);
        static void SaveBaseDataEntity(Entity *const &entity, nlohmann::json_abi_v3_12_0::json &entityData);
        static void LoadEntityScript(const nlohmann::json_abi_v3_12_0::json &script, Entity *entity, IScript* existingScript = nullptr);
        void ExtractScriptVariables(std::vector<ExposedVariable *> &exposedVariables, IScript *scriptInstance, Entity *entity, std::string &scriptName);
        static void ExtractExposedVariable(const nlohmann::json_abi_v3_12_0::json &var, std::vector<ExposedVariable *> &exposedVariables);
        static Entity* LoadEntityBaseData(const nlohmann::json_abi_v3_12_0::json &entityData);
        
};

#endif