#include "ModuleLoader.h"

IModule *ModuleLoader::GetModuleFromPath(const std::string &modulePath)
{
    HMODULE moduleHandle = LoadLibrary(modulePath.c_str());
    if (!moduleHandle) {
        std::cerr << "Error loading module: " << modulePath << std::endl;
        return nullptr;
    }

    CreateModuleFunc createModuleFunc = (CreateModuleFunc)GetProcAddress(moduleHandle, "CreateModule");
    if (!createModuleFunc) {
        std::cerr << "Error finding CreateModule function in module: " << modulePath << std::endl;
        FreeLibrary(moduleHandle);
        return nullptr;
    }

    IModule *moduleInstance = createModuleFunc();
    if (!moduleInstance) {
        std::cerr << "Error creating module instance from: " << modulePath << std::endl;
        FreeLibrary(moduleHandle);
        return nullptr;
    }

    return moduleInstance;

}