#ifndef FILEREADER_H
#define FILEREADER_H

#include "pch.h"
#include "Common/common.h"
#include "Common/dllExport.h"

class PULSE_ENGINE_DLL_API FileReader
{
    public:
        FileReader(const std::string& path);
        ~FileReader();

        nlohmann::json ToJson();
        


    private:
        void* fileData;

        template<typename T>
        T* ReinterprateFileType()
        {
            return static_cast<T*>(fileData);
        }
    
};

#endif