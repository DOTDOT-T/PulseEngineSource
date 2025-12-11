#include "Common/common.h"
#include "ShaderManager.h"
#include "json.hpp"
PULSE_REGISTER_CLASS_CPP(ShaderManager)

std::unordered_map<std::string, ShaderInfo> ShaderManager::shaders; 
std::unordered_map<std::string, Shader*> ShaderManager::shadersInstance; 

void ShaderManager::Serialize(Archive &ar)
{
}
void ShaderManager::Deserialize(Archive &ar)
{
}
const char *ShaderManager::ToString()
{
    return GetTypeName();
}

ShaderManager::ShaderManager()
{

    std::string collectionPath = std::string(ASSET_PATH) + "EngineConfig/Guid/GuidCollectionShaders.puid";
    nlohmann::json jsonData;
    std::ifstream inFile(collectionPath);

    if (!inFile.is_open())
    {
        EDITOR_ERROR("Failed to open GUID collection file.");
        return;
    }

    inFile >> jsonData;

    for (const auto &item : jsonData.items())
    {
        const auto &obj = item.value();

        ShaderInfo info;
        info.name = obj.value("name", "");
        info.vert = obj.value("vert", "");
        info.frag = obj.value("frag", "");
        info.guid = item.key();

        shaders[item.key()] = info;
    }
}

void ShaderManager::SaveToFile()
{
    nlohmann::json jsonData;

    for (const auto &pair : shaders)
    {
        const std::string &guid = pair.first;
        const ShaderInfo &info = pair.second;

        jsonData[guid] = {
            {"name", info.name},
            {"vert", NormalizePath(info.vert)},
            {"frag", NormalizePath(info.frag)}};
    }

    std::ofstream outFile(std::string(ASSET_PATH) + "EngineConfig/Guid/GuidCollectionShaders.puid");
    if (outFile.is_open())
    {
        outFile << jsonData.dump(4);
        outFile.close();
    }
    else
    {
        EDITOR_ERROR("Failed to open file for writing: EngineConfig/Guid/GuidCollectionShaders.puid");
    }
}

Shader *ShaderManager::GetShaderInstance(const std::string &guid)
{
    auto it = shadersInstance.find(guid);
    if (it != shadersInstance.end())
    {
        return it->second;
    }

    auto shaderIt = shaders.find(guid);
    if (shaderIt != shaders.end())
    {
        const ShaderInfo &info = shaderIt->second;
        Shader *shader = new Shader(std::string(ASSET_PATH) + info.vert, std::string(ASSET_PATH) + info.frag, PulseEngineGraphicsAPI);
        shader->shaderName = info.name;
        shader->guid = info.guid;
        shadersInstance[guid] = shader;
        return shader;
    }

    return nullptr;
}

std::string ShaderManager::NormalizePath(const std::string& input)
{
    std::string out;
    out.reserve(input.size());

    bool lastWasSlash = false;

    for (char c : input)
    {
        char normalized = (c == '\\') ? '/' : c;

        if (normalized == '/')
        {
            if (lastWasSlash)
                continue;           // skip double slash

            lastWasSlash = true;
        }
        else
        {
            lastWasSlash = false;
        }

        out.push_back(normalized);
    }

    //if it start with PulseEngineEditor/ remove it
    const std::string prefix = "PulseEngineEditor/";
    if (out.rfind(prefix, 0) == 0)
    {
        out = out.substr(prefix.length());
    }

    return out;
}