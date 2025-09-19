#include "ScriptsLoader.h"
#include <filesystem>

typedef IScript* (*CreateScriptFunc)();

HMODULE ScriptsLoader::customScriptDll = nullptr;
HMODULE ScriptsLoader::pulseCustomScriptDll = nullptr;
std::map<std::string, CreateScriptFunc> ScriptsLoader::scriptMap;


IScript *ScriptsLoader::GetScriptFromCallName(std::string callName)
{ 
    if(!customScriptDll) LoadDLL();
    IScript* toReturn = nullptr;
    if(scriptMap[callName]) toReturn = scriptMap[callName]();
    if(!toReturn)
    {        
        scriptMap[callName] = (CreateScriptFunc)GetProcAddress(customScriptDll, callName.c_str());
        if (!scriptMap[callName])
        {
            std::cerr << "Impossible de trouver " << callName << " dans la DLL ! " << scriptMap[callName] << std::endl;
            return nullptr;
        }
        toReturn = scriptMap[callName](); 
    }
    return toReturn;
}

void ScriptsLoader::LoadDLL()
{
    std::cout << "Working directory: " << std::filesystem::current_path() << std::endl;
    std::string dllDirectory = "";
    EDITOR_ONLY(dllDirectory = "";)
    customScriptDll = LoadLibrary("CustomScripts.dll");
    pulseCustomScriptDll = LoadLibrary("pulseCustomScripts.dll");
    if (customScriptDll == nullptr) 
    {
        DWORD error = GetLastError(); // Récupère l'erreur système
        std::cerr << "Error loading CustomScripts.dll, error code: " << error << std::endl;
    }
    else
    {
        LoadExportedFunctions(customScriptDll);
        std::cout << "CustomScripts.dll loaded without errors" << std::endl;
    }
    if (pulseCustomScriptDll == nullptr) 
    {
        DWORD error = GetLastError(); // Récupère l'erreur système
        std::cerr << "Error loading pulseCustomScript.dll, error code: " << error << std::endl;
    }
    else
    {
        LoadExportedFunctions(pulseCustomScriptDll);
        std::cout << "pulseCustomScript.dll loaded without errors" << std::endl;
    }
}

void ScriptsLoader::FreeDll()
{
        FreeLibrary(customScriptDll); // Libère la DLL après le chargement des fonctions
        customScriptDll = nullptr; // Réinitialise le handle de la DLL
}

void ScriptsLoader::LoadExportedFunctions(HMODULE dll)
{
    DWORD size;
    BYTE* base = (BYTE*)dll;

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
    IMAGE_EXPORT_DIRECTORY* exportDir = (IMAGE_EXPORT_DIRECTORY*)(
        base + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    DWORD* funcNames = (DWORD*)(base + exportDir->AddressOfNames);
    DWORD* funcAddrs = (DWORD*)(base + exportDir->AddressOfFunctions);
    WORD* nameOrdinals = (WORD*)(base + exportDir->AddressOfNameOrdinals);

    for (size_t i = 0; i < exportDir->NumberOfNames; ++i)
    {
        const char* funcName = (const char*)(base + funcNames[i]);

        // Si le nom commence par "PulseScript"
        if (std::string(funcName).rfind("PulseScript", 0) == 0)
        {
            FARPROC proc = GetProcAddress(dll, funcName);
            if (proc)
            {
                scriptMap[funcName] = (CreateScriptFunc)proc;
                std::cout << "Script function loaded: " << funcName << std::endl;
            }
        }
    }
}


