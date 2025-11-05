#ifndef ISCRIPT_H
#define ISCRIPT_H

#include "Common/dllExport.h"
#include <unordered_map>
#include <string>
#include <vector>

//ExposedVariable is used to expose variables to the editor
#define EXPOSE_VAR(var, typeEnum) ExposedVariable(#var, ExposedVariable::Type::typeEnum, reinterpret_cast<void*>(&var))
#define REGISTER_VAR(var) RegisterRuntimeVariable(#var, reinterpret_cast<void*>(&var))

namespace PulseEngine
{
    class EntityApi;
}

struct ExposedVariable
{
    enum class Type
    {
        INT,
        FLOAT,
        BOOL,
        STRING
    };

    std::string name;
    Type type;
    void* ptr;
    
    ExposedVariable(const std::string& name, Type type, void* ptr)
        : name(name), type(type), ptr(ptr) {}
};

class PULSE_ENGINE_DLL_API IScript
{
private:
    std::size_t guid;
public:
    virtual void OnStart() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;

    virtual void OnEditorDisplay() = 0;

    virtual ~IScript() = default;
    virtual const char* GetName() const = 0;
    std::vector<ExposedVariable> GetExposedVariables() {return exposedVariables; }
    std::size_t GetGUID() const { return guid; }
    void SetGUID(std::size_t newGuid) { guid = newGuid; }

    PulseEngine::EntityApi* owner;
    bool isEntityLinked = false;


    std::vector<ExposedVariable> exposedVariables;
    std::unordered_map<std::string, void*> runtimeVariables;
    void AddExposedVariable(const ExposedVariable& var)
    {
        exposedVariables.push_back(var);
    }

    void RegisterRuntimeVariable(const std::string& name, void* ptr)
    {
        runtimeVariables[name] = ptr;
    }

    void* GetVariableByName(const std::string& name)
    {
        auto it = runtimeVariables.find(name);
        if (it != runtimeVariables.end())
        {
            return it->second;
        }
        return nullptr;
    }

};



#endif
