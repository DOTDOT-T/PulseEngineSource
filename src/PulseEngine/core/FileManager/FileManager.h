/**
 * @file FileManager.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Filemanager to handle file operations in the Pulse Engine, with a cross-platform approach.
 * @version 0.1
 * @date 2025-06-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

// #include "Common/common.h"
#include "Common/dllExport.h"
#include "Common/EditorDefines.h"
#include "json.hpp"

#include <string>
#include <filesystem>

class PulseEngineBackend;

namespace PulseEngine::FileSystem
{

    enum FileType
    {
        TEXTURE,
        MESH,
        PULSE_ENTITY,
        MATERIAL,
        MAP,
        SCRIPT_CPP,
        SCRIPT_H,
        SYNAPSE,
        UNKNOWN
    };

    class PULSE_ENGINE_DLL_API FileManager
    {
    public:
        private:
        FileManager() = default;
        ~FileManager() = default;

        public:
        static std::filesystem::path workingDirectory;
        /**
         * Returns the collection type based on the file extension.
         */
        static std::string GetCollectionByExtension(const std::string& fileName);
        /**
        * Loads engine configuration from disk into a JSON object.
        */
        static nlohmann::json_abi_v3_12_0::json OpenEngineConfigFile();
        /**
        * Saves engine configuration from a JSON object to disk.
        */
        static void SaveEngineConfigFile(const nlohmann::json_abi_v3_12_0::json& engineConfig);

        /**
         * @brief Get the File Type object based on the file extension in the path (or file name)
         * 
         * @param fileName the path or the name WITH the extension.
         * @return FileType of the file (TEXTURE, MESH, PULSE_ENTITY, MATERIAL, MAP, SCRIPT_CPP, SCRIPT_H, UNKNOWN)
         */
        static FileType GetFileType(const std::string& fileName);


            /**
         * @brief Get all files in a directory with a specific extension.
         * @param directory The directory to search in.
         * @param extension The file extension to filter by (e.g., ".puid").
         * @return std::vector<std::filesystem::path> List of matching files.
         */
        static std::vector<std::filesystem::path> GetFilesInDirectoryWithExtension(const std::string& directory, const std::string& extension);

        /**
         * @brief Create New Directories recursively if they do not exist.
         * 
         * @param path 
         * @return true if they got all created or already exist
         * @return false if an error occurred
         */
        static bool CreateNewDirectories(const std::string& path);    
    };
}


#endif