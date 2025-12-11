#ifndef __SHADERMANAGER_H__
#define __SHADERMANAGER_H__

#include "PulseEngine/core/PulseObject/PulseObject.h"
#include "PulseEngine/core/PulseObject/TypeRegister/TypeRegister.h"
#include "shader.h"
#include <string>
#include <unordered_map>
#include "Common/dllExport.h"
#include "Common/EditorDefines.h"
#include "PulseEngine/core/GUID/GuidGenerator.h"


struct ShaderInfo 
{
    std::string name;
    std::string vert;
    std::string frag;
    std::string guid;
};


class PULSE_ENGINE_DLL_API ShaderManager : public PulseObject
{
PULSE_GEN_BODY(ShaderManager)
PULSE_REGISTER_CLASS_HEADER(ShaderManager)
public:
    ShaderManager();

    static ShaderManager& GetInstance()
    {
        static ShaderManager instance;
        return instance;
    }

    std::unordered_map<std::string, ShaderInfo>& GetAllShaders() const { return shaders; }

    std::uint64_t RegisterShader(const std::string& guid, ShaderInfo info)
    {
        std::uint64_t guidgen =  PulseEngine::Registry::GenerateGUIDFromPath(guid);
        while(shaders.find(std::to_string(guidgen)) != shaders.end())
        {
            guidgen++;
        }
        info.guid = std::to_string(guidgen);
        shaders[info.guid] = info;

        SaveToFile();
        return guidgen;
    }

    void SaveToFile();

    Shader* GetShaderInstance(const std::string& guid);

private:

    static std::unordered_map<std::string, ShaderInfo> shaders; // key: shader GUID
    static std::unordered_map<std::string, Shader*> shadersInstance; // key: shader GUID

    std::string NormalizePath(const std::string& input);

};



#endif // __SHADERMANAGER_H__