#include "PulseScriptsManager.h"
#include "PulseInterpreter.h"
#include "PulseScript.h"

void PulseScriptsManager::AddScriptToDatabase(const std::string& scriptPath)
{
        scripts.emplace(scriptPath, new PulseScript(scriptPath.c_str()));
}

PulseScript* PulseScriptsManager::GetScript(const std::string& scriptName)
{
    // Cherche si déjà chargé
    auto it = scripts.find(scriptName);
    if (it != scripts.end())
        return it->second;

    try
    {
        PulseScript* script = new PulseScript(scriptName.c_str());

        scripts.emplace(scriptName, script);
        return script;
    }
    catch (const std::exception&)
    {
        return nullptr;
    }
}


bool PulseScriptsManager::ExecuteScript(const std::string &scriptName)
{
    PulseScript* script = GetScript(scriptName);
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
