#include "PulseScript.h"

#include <iostream>
#include <fstream>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "PulseLexer.h"
#include "PulseParser.h"
#include "PulseInterpreter.h"

PulseScript::PulseScript(const char *scriptPath)
{
    std::string code = ReadFileToString("main.PulseScript");    

    // Lexer
    PulseLexer lexer(code);
    std::vector<Token> tokens;
    while (!lexer.End()) tokens.push_back(lexer.Next());
    tokens.push_back({TokenType::EndOfFile, ""});

    // Parser
    PulseParser parser(tokens);
    auto src = parser.ParseScript();
    ast = std::move(src);

    itp = std::make_shared<PulseInterpreter>();
    
    std::vector<std::unique_ptr<ASTStatement>> clonedAst;
    for (auto& stmt : ast) {
        clonedAst.push_back(itp->CloneStatement(stmt.get()));
    }

    itp->DeclareGlobalVariable(std::move(clonedAst));

}

PulseScript::~PulseScript()
{
}

void PulseScript::Execute()
{
    itp->Execute(ast);
}

void PulseScript::ExecuteScriptFunction(const char *functionName, const std::vector<Variable> &args)
{
    itp->ExecuteFunction(std::string(functionName), args, ast);
}

std::string PulseScript::ReadFileToString(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Impossible d'ouvrir le fichier: " + filename);
    }

    std::ostringstream ss;
    ss << file.rdbuf();  // Lit tout le contenu
    return ss.str();
}
