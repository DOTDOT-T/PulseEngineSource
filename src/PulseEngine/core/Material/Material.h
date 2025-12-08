#ifndef MATERIAL_H
#define MATERIAL_H

#include "Common/common.h"
#include <string>
#include <unordered_map>
#include <memory>

#include "Common/dllExport.h"
#include "PulseEngine/core/Math/Vector.h"

class Shader;
class Texture;

class PULSE_ENGINE_DLL_API Material
{
public:
    Material(const std::string& name, Shader* shader) : name(name), shader(shader) {}

    Shader* GetShader();

    void SetName(const std::string& name) { this->name = name; }
    const std::string GetName() const { return name; }

    // --- Propriétés de base
    float roughness = 10.0f;
    float specular = 10.0f;
    std::string guid;
    PulseEngine::Vector3 color = PulseEngine::Vector3(1.0f, 1.0f, 1.0f);

    // --- Textures ---
    void SetTexture(const std::string& type, std::shared_ptr<Texture> texture)
    {
        textures[type] = texture;
    }
    void SetShader(Shader* shader);


    std::shared_ptr<Texture> GetTexture(const std::string& type) const
    {
        auto it = textures.find(type);
        if (it != textures.end())
        {
            return it->second;
        }
        return nullptr;
    }

    const std::unordered_map<std::string, std::shared_ptr<Texture>>& GetAllTextures() const
    {
        return textures;
    }

    std::string GetPath() const {return path;}
    void SetPath(const std::string& path) { this->path = path; }

    bool HasYFlip() {return needYFlip;}
    bool* GetFlipPtr() {return &needYFlip;}
    void SetYflip(bool hasFlip) {needYFlip = hasFlip;}

private:
    Shader* shader;
    std::string name;
    std::string path;
    bool needYFlip = false;

    std::unordered_map<std::string, std::shared_ptr<Texture>> textures; // "albedo", "normal", etc.
};

#endif
