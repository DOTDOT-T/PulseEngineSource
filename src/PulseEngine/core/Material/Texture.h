// Texture.h
#pragma once

#include <string>
// #include "Common/common.h"
#include "Common/dllExport.h"

class IGraphicsAPI;

class PULSE_ENGINE_DLL_API Texture
{
public:
    Texture(const std::string& filePath,IGraphicsAPI* graphics, bool hasFlip = false);

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    std::string GetPath() const { return path; }

    unsigned int id;
private:
    std::string path;
    IGraphicsAPI* graphicsAPI;
};
