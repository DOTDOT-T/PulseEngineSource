#ifndef __PULSESCRIPTSMANAGER_H__
#define __PULSESCRIPTSMANAGER_H__

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "utilities.h"

class PulseScript;
class PulseInterpreter;

class PulseScriptsManager
{
public:
    void AddScriptToDatabase(const std::string& scriptPath);
    PulseScript* GetScript(const std::string& scriptName);
    bool ExecuteScript(const std::string& scriptName);

    bool ExecuteMethodOnEachScript(const char* methodName, std::vector<Variable> args);

private:
    std::unordered_map<std::string, PulseScript*> scripts; // name -> script
};

#endif // __PULSESCRIPTSMANAGER_H__