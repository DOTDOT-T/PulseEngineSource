/**
 * @file GuidReader.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef GUIDREADER_H
#define GUIDREADER_H

#include "Common/common.h"
#include "Common/dllExport.h"

#include <assimp/Importer.hpp>      // Assimp::Importer
#include <assimp/scene.h>           // aiScene
#include <assimp/postprocess.h>     // postprocessing flags

class Entity;
class Mesh;
class Material;
class RenderableMesh;
class SkeletalMesh;


class PULSE_ENGINE_DLL_API GuidReader
{
    public:
        /**
         * @brief If you already have the GUID of the entity wanted to be loaded.
         * @note The GUID is the one generated from the engine and saved inside the collection.
         * 
         * @param Guid actual GUID (unique identifier) of the entity saved in the engine
         * @return Entity* a new entity from your wanted one.
         */
        static Entity* GetEntityFromGuid(std::size_t Guid);
        /**
         * @brief If you already have the GUID of the mesh wanted to be loaded.
         * @note The GUID is the one generated from the engine and saved inside the collection.
         * 
         * @param Guid actual GUID (unique identifier) of the mesh saved in the engine
         * @return Mesh* a new mesh from your wanted one.
         */
        static RenderableMesh* GetMeshFromGuid(std::size_t guid);
        
        /**
         * @brief If you have the path to the entity, use this one. It's way faster than using the GUID since it didn't need to find it in the engine.
         * 
         * @param entityData the actual json data of the entity.
         * @param entity the entity to fill with the data from the json.
         * @return Entity* its the same as the one pass to the function.
         */
        static Entity *GetEntityFromJson(nlohmann::json_abi_v3_12_0::json &entityData, Entity *entity);
        /**
         * @brief Get the content of a material via the actual json (.material) from the engine material saved file.
         * 
         * @param materialData the actual json
         * @return Material* a new material from the json data.
         */
        static Material* GetMaterialFromJson(nlohmann::json_abi_v3_12_0::json &materialData);
        /**
         * @brief If you've got data to add to the collection, to have a new GUID, use this function. It just need a filepath. It can be used for anything that is not related to engine files (like .material, .mesh, etc.)
         * 
         * @param filePath 
         * @return std::size_t 
         */
        static std::size_t InsertIntoCollection(const std::string& filePath);
        /**
         * @brief Get the files from a guid collection.
         * 
         * @param guidFile 
         * @return std::vector<std::pair<std::string, std::string>> you've got all the path available inside a collection.
         */
        static std::vector<std::pair<std::string, std::string>> GetAllAvailableFiles(const std::string& guidFile = ".pEntity");
        static void LoadSkeletonFromAssimp(SkeletalMesh* skel, const aiScene* scene);

        static void LoadAnimationsFromAssimp(SkeletalMesh* skel, const aiScene* scene);

};

#endif