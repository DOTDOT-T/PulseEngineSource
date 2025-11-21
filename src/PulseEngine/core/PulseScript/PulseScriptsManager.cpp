#include "PulseScriptsManager.h"
#include "PulseInterpreter.h"
#include "PulseScript.h"

std::unordered_map<std::string, std::shared_ptr<PulseScript>> PulseScriptsManager::scripts;

void PulseScriptsManager::AddScriptToDatabase(const std::string& scriptPath)
{
    scripts.emplace(scriptPath, std::make_shared<PulseScript>(scriptPath.c_str()));
}

std::shared_ptr<PulseScript> PulseScriptsManager::GetScript(const std::string& scriptName)
{
    auto it = scripts.find(scriptName);
    if (it != scripts.end())
        return it->second;

    try 
    {
        auto script = std::make_shared<PulseScript>(scriptName.c_str());
        scripts.emplace(scriptName, script);
        return script;
    }
    catch (const std::exception& e) 
    {
        return nullptr;
    }
}

bool PulseScriptsManager::ExecuteScript(const std::string &scriptName)
{
    std::shared_ptr<PulseScript> script = GetScript(scriptName);
    if(script)
    {
        script->Execute();
        return true;
    }

    return false;
}

bool PulseScriptsManager::ExecuteMethodOnEachScript(const char* methodName, std::vector<Variable> args)
{
    for(auto& script : scripts)
    {
        script.second->ExecuteScriptFunction(methodName, args);
    }

    return true;
}
