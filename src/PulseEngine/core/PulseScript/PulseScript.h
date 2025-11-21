#ifndef PULSESCRIPT_H
#define PULSESCRIPT_H

#include <memory>
#include <vector>
#include <string>
#include "utilities.h"

class PulseLexer;
class PulseParser;
class PulseInterpreter;
struct ASTStatement;

class PulseScript
{
public:
    PulseScript(const char* scriptPath);
    ~PulseScript();
    void Execute();
    void ExecuteScriptFunction(const char* functionName, const std::vector<Variable> &args);

private:
    std::vector<std::unique_ptr<ASTStatement>> ast;
    std::shared_ptr<PulseInterpreter> itp;
    std::string ReadFileToString(const std::string& filename);
};

#endif