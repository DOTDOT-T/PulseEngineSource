#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <string>
#include <unordered_map>
#include "Common/dllExport.h"

class Material;

class PULSE_ENGINE_DLL_API MaterialManager 
{
public:

    static Material* loadMaterial(const std::string& filePath);
    static Material* getMaterial(const std::string& materialName);
    static void unloadMaterial(const std::string& materialName);
private:
    static std::unordered_map<std::string, Material*> materials;
};


#endif