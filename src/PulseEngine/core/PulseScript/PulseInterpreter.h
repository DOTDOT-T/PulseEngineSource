/**
 * @file PulseInterpreter.h
 * @author
 *     Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 *
 * @brief
 *     Runtime execution engine for the PulseScript language.
 *
 *     The PulseInterpreter walks the AST produced by the parser and executes
 *     the program in real-time. It manages variable scopes, user-defined
 *     functions, native bindings, and expression evaluation with strict
 *     type rules.
 *
 *     Designed for seamless integration inside the Pulse Engine, the
 *     interpreter enables high-level gameplay scripting with minimal overhead.
 *     Its approach favors predictable behavior, straightforward debugging,
 *     and integration with engine-level systems (logging, entity access,
 *     real-time variables, etc.).
 *
 *     Core responsibilities:
 *       - Evaluate expressions (numeric, float, string, identifiers, binary ops)
 *       - Execute statements (let, function definitions, function calls)
 *       - Maintain execution scope for variables
 *       - Manage user-defined function contexts and call stacks
 *       - Expose native engine functions to scripts
 *
 *     The interpreter is intentionally lightweight and single-pass, enabling
 *     the Pulse Engine to run scripted logic every frame, during events,
 *     or inside editor tools without performance penalties.
 *
 * @version 0.2
 * @date 2025-11-20
 *
 * @copyright
 *     Copyright (c) 2025 — Pulse Engine
 *     All rights reserved.
 *
 */

#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "PulseParser.h"



class PulseInterpreter
{
public:
    PulseInterpreter();


    // exécution d'une liste de statements
    void Execute(const std::vector<std::unique_ptr<ASTStatement>> &stmts);

    /**
     * @brief Declare data of the scope : variable and function that are "global".
     * 
     * @param stmts 
     */
    void DeclareGlobalVariable(const std::vector<std::unique_ptr<ASTStatement>> &stmts);
    
    void DeclareVariable(ASTLetStatement *letStmt);
    void GenerateUserFunctions(ASTFunctionDef *fdef);

    void ExecuteFunction(ASTFunctionDef *func, const std::vector<Variable> &args);
    void ExecuteFunction(const std::string& func, const std::vector<Variable> &args, const std::vector<std::unique_ptr<ASTStatement>> &stmts);
    // fonctions natives
    static void RegisterFunction(const std::string &name, std::function<Value(const std::vector<Value> &)> func);

    const Scope &GetScope() const { return scope; }

    std::unique_ptr<ASTExpression> CloneExpression(const ASTExpression* expr);
    std::unique_ptr<ASTStatement> CloneStatement(const ASTStatement* stmt);
private:
    Value EvalExpression(const ASTExpression *expr);

    Scope scope;
    static std::unordered_map<std::string, std::function<Value(const std::vector<Value> &)>> nativeFunctions;
    std::unordered_map<std::string, std::unique_ptr<ASTFunctionDef>> userFunctions;
};
