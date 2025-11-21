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
    static void AddScriptToDatabase(const std::string& scriptPath);
    static std::shared_ptr<PulseScript> GetScript(const std::string& scriptName);
    static bool ExecuteScript(const std::string& scriptName);

    static bool ExecuteMethodOnEachScript(const char* methodName, std::vector<Variable> args);

private:
    static std::unordered_map<std::string, std::shared_ptr<PulseScript>> scripts; // name -> script
};

#endif // __PULSESCRIPTSMANAGER_H__