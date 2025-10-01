/**
 * @file FileReader.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Simple class for reading files.
 * @version 0.1
 * @date 2025-09-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef FILEREADER_H
#define FILEREADER_H

#include "pch.h"
#include "Common/common.h"
#include "Common/dllExport.h"

/**
 * @brief This class is used for an easy to use files reading regardless of the platform.
 * It currently supports reading files as JSON objects using the nlohmann::json library.
 * @note the path to use is relative to "PulseEngineEditor/". so your file 'Assets/myfile.pconfig' will be search at 'PulseEngineEditor/Assets/myfile.pconfig'
 */
class PULSE_ENGINE_DLL_API FileReader
{
    public:
        FileReader(const std::string& path);
        ~FileReader();

        nlohmann::json ToJson();
        void SaveJson(const nlohmann::json& js);

        bool IsOpen();
        void Close();
        


    private:
        void* fileData;
        std::string filePath;

        /**
         * @brief This function reinterprate the void* fileData to the final type wanted.
         * @note make sure your final variable is a pointer and the type in the function template is not a pointer.
         * @note an example : std::ifstream* file = ReinterprateFileType<std::ifstream>();
         * 
         * @return T* the reinterpreted pointer of fileData.
         */
        template<typename T>
        T* ReinterprateFileType()
        {
            return static_cast<T*>(fileData);
        }
    
};

#endif