#include "PulseInterpreter.h"
#include <iostream>
#include <stdexcept>

#include <iostream>

std::unordered_map<std::string, std::function<Value(const std::vector<Value> &)>> PulseInterpreter::nativeFunctions;

PulseInterpreter::PulseInterpreter()
{

    // Exemple : fonction log
}

void PulseInterpreter::DeclareGlobalVariable(const std::vector<std::unique_ptr<ASTStatement>> &stmts)
{
    for (auto &stmt : stmts)
    {
        if (auto letStmt = dynamic_cast<ASTLetStatement *>(stmt->content.get()))
        {
            // exécution d'un let
            DeclareVariable(letStmt);
        }
        else if (auto fdef = dynamic_cast<ASTFunctionDef *>(stmt->content.get()))
        {
            GenerateUserFunctions(fdef);
        }
    }
}

void PulseInterpreter::RegisterFunction(const std::string &name, std::function<Value(const std::vector<Value> &)> func)
{
    nativeFunctions[name] = func;
}

void PulseInterpreter::Execute(const std::vector<std::unique_ptr<ASTStatement>> &stmts)
{
    for (auto &stmt : stmts)
    {
        if (auto letStmt = dynamic_cast<ASTLetStatement *>(stmt->content.get()))
        {
            // exécution d'un let
            DeclareVariable(letStmt);
        }
        // else if (auto fdef = dynamic_cast<ASTFunctionDef *>(stmt->content.get()))
        // {
        //     GenerateUserFunctions(fdef);
        // }
        if (auto call = dynamic_cast<ASTFunctionCall *>(stmt->content.get()))
        {
            // appel de fonction
            auto it = userFunctions.find(call->name);
            if (it != userFunctions.end())
            {
                std::vector<Variable> args;
                for (auto &a : call->args)
                {
                    ASTIdentifier* id = dynamic_cast<ASTIdentifier*>(a.get());
                    if(id) args.push_back({.value = EvalExpression(a.get()), .name = id->name});
                    else args.push_back({.value = EvalExpression(a.get())});
                }
                ExecuteFunction(it->second.get(), args);
            }
            else
            {
                // fonction native
                auto itNative = nativeFunctions.find(call->name);
                if (itNative != nativeFunctions.end())
                {
                    std::vector<Value> args;
                    for (auto &a : call->args)
                        args.push_back(EvalExpression(CloneExpression(a.get()).get()));
                    itNative->second(args);
                }
                else
                {
                    throw std::runtime_error("Unknown function: " + call->name);
                }
            }
        }
    }
}

void PulseInterpreter::DeclareVariable(ASTLetStatement *letStmt)
{
    Value val = EvalExpression(letStmt->value.get());
    if(!scope.variables.contains(letStmt->varName)) scope.variables[letStmt->varName] = {.value = val};
    else scope.variables[letStmt->varName].value = val;
    
}

void PulseInterpreter::GenerateUserFunctions(ASTFunctionDef *fdef )
{
    userFunctions[fdef->name] = std::make_unique<ASTFunctionDef>();
    userFunctions[fdef->name]->name = fdef->name;
    userFunctions[fdef->name]->parameters = fdef->parameters;

    // déplacer le body
    for (auto &s : fdef->body)
    {
        userFunctions[fdef->name]->body.push_back(CloneStatement(s.get()));
    }
}

void PulseInterpreter::ExecuteFunction(ASTFunctionDef *func, const std::vector<Variable> &args)
{
    if (args.size() != func->parameters.size())
        throw std::runtime_error("Wrong number of arguments in function call");

    // Create a new heap-allocated snapshot of the current scope so the
    // function's `scope.parent` can point to a stable address that remains
    // valid across nested calls. Using a stack-local `previous` allowed
    // parent to point into the stack which becomes invalid for nested calls.
    auto previousPtr = std::make_unique<Scope>(scope); // copy previous scope snapshot

    // Create a fresh local scope for the function and attach the previous
    // snapshot as its parent so lookups fall back to the caller scope.
    Scope local;
    local.parent = previousPtr.get();
    scope = std::move(local);

    // Bind parameters
    for (size_t i = 0; i < args.size(); i++)
    {
        Variable var;
        // If caller provided a named variable (identifier), args[i].name contains it.
        // For reference parameters we want to be able to map back to that original
        // name so we store it in `var.name` for synchronization after the call.
        var.value = args[i].value;
        var.name = args[i].name; // original caller variable name (may be empty)
        var.isGlobal = false;
        var.passMethod = func->parameters[i].passMethod;
        // store under the parameter name inside the local scope
        scope.variables[func->parameters[i].name] = var;
    }
    // add local variable to scope    
    for (auto& stmt : func->body) 
    {
        if (auto letStmt = dynamic_cast<ASTLetStatement*>(stmt->content.get())) 
        {
            if(scope.variables.contains(letStmt->varName)) continue;
            Value val = EvalExpression(letStmt->value.get());
            scope.variables[letStmt->varName] = {.value = val, .name = letStmt->varName };
        }
    }

    // Execute body
    Execute(func->body);

    // If code modified variables marked as global in the local scope, write
    // them back to the previous snapshot using Scope::Set so parent lookup
    // and creation semantics are respected.
    for (auto &pair : scope.variables)
    {
        const std::string &localName = pair.first;
        const Variable &localVar = pair.second;
        if (localVar.isGlobal)
        {
            previousPtr->Set(localName, localVar);
        }
    }

    // Synchronize reference parameters back to the caller variables.
    // For parameters passed by reference we stored the original caller name
    // in `var.name` when binding; use that to update the caller snapshot.
     for (auto &param : func->parameters)
     {
         if (param.passMethod == ParamPassMethod::REFERENCE)
         {
             auto it = scope.variables.find(param.name);
             if (it == scope.variables.end())
                 continue;
             const Variable &boundVar = it->second;
             if (!boundVar.name.empty())
             {
                 // Update the original variable (by its original name) in the
                 // previous snapshot so changes are visible after restoring.
                 previousPtr->Set(boundVar.name, boundVar);
             }
         }
     }

    // Restore previous scope (move the stored snapshot back into member)
    scope = std::move(*previousPtr);
}

void PulseInterpreter::ExecuteFunction(const std::string &func, const std::vector<Variable> &args, const std::vector<std::unique_ptr<ASTStatement>> &stmts)
{

    for (auto &stmt : stmts)
    {
        if (auto fdef = dynamic_cast<ASTFunctionDef *>(stmt->content.get()))
        {       
            if(fdef->name != func) continue;         
            GenerateUserFunctions(fdef);
            break;
        }
    }
        
    
    ExecuteFunction(userFunctions[func].get(), args);
}

Value PulseInterpreter::EvalExpression(const ASTExpression *expr)
{
    if (!expr)
        throw std::runtime_error("Null expression");
    if (auto call = dynamic_cast<const ASTFunctionCall *>(expr))
    {
        std::vector<Value> args;
        for (auto &arg : call->args)
        {
            args.push_back(EvalExpression(arg.get()));
        }
        auto it = nativeFunctions.find(call->name);
        if (it != nativeFunctions.end())
            return it->second(args);
        throw std::runtime_error("Unknown function: " + call->name);
    }
    if (auto bin = dynamic_cast<const ASTBinaryOp *>(expr))
    {
        auto leftVal = EvalExpression(bin->left.get());
        auto rightVal = EvalExpression(bin->right.get());

        float leftF, rightF;

        // Convert left operand
        if (auto pInt = std::get_if<int>(&leftVal))
            leftF = static_cast<float>(*pInt);
        else if (auto pFloat = std::get_if<float>(&leftVal))
            leftF = *pFloat;
        else
            throw std::runtime_error("Left operand is not numeric");

        // Convert right operand
        if (auto pInt = std::get_if<int>(&rightVal))
            rightF = static_cast<float>(*pInt);
        else if (auto pFloat = std::get_if<float>(&rightVal))
            rightF = *pFloat;
        else
            throw std::runtime_error("Right operand is not numeric");

        // Compute
        float result;
        switch (bin->op) {
            case '+': result = leftF + rightF; break;
            case '-': result = leftF - rightF; break;
            case '*': result = leftF * rightF; break;
            case '/': 
                if (rightF == 0) throw std::runtime_error("Division by zero");
                result = leftF / rightF; 
                break;
            default:
                throw std::runtime_error("Unknown binary operator");
        }

        // If both were int, maybe return int
        if (std::holds_alternative<int>(leftVal) && std::holds_alternative<int>(rightVal))
            return static_cast<int>(result);
        return result;

    }

    return expr->Evaluate(scope);

    throw std::runtime_error("Unknown expression type");
}

std::unique_ptr<ASTExpression> PulseInterpreter::CloneExpression(const ASTExpression *expr)
{
    if (!expr) return nullptr;

    if (auto n = dynamic_cast<const ASTNumber*>(expr))
    {
        return std::make_unique<ASTNumber>(n->value);
    }
    if (auto n = dynamic_cast<const ASTFloatingNumber*>(expr))
    {
        return std::make_unique<ASTFloatingNumber>(n->value);
    }
    if (auto s = dynamic_cast<const ASTString*>(expr))
    {
        return std::make_unique<ASTString>(s->value);
    }
    if (auto id = dynamic_cast<const ASTIdentifier*>(expr))
    {
        return std::make_unique<ASTIdentifier>(id->name);
    }
    if (auto call = dynamic_cast<const ASTFunctionCall*>(expr))
    {
        auto callCopy = std::make_unique<ASTFunctionCall>();
        callCopy->name = call->name;
        for (const auto &arg : call->args)
            callCopy->args.push_back(CloneExpression(arg.get()));
        return callCopy;
    }
    if (auto bin = dynamic_cast<const ASTBinaryOp*>(expr))
    {
        return std::make_unique<ASTBinaryOp>(
            bin->op,
            CloneExpression(bin->left.get()),
            CloneExpression(bin->right.get())
        );
    }

    throw std::runtime_error("Unknown expression type in CloneExpression");
}

std::unique_ptr<ASTStatement> PulseInterpreter::CloneStatement(const ASTStatement *stmt)
{
    if (!stmt) return nullptr;

    auto copy = std::make_unique<ASTStatement>();

    if (auto letStmt = dynamic_cast<const ASTLetStatement*>(stmt->content.get()))
    {
        auto letCopy = std::make_unique<ASTLetStatement>();
        letCopy->varName = letStmt->varName;
        letCopy->value = CloneExpression(letStmt->value.get());
        copy->content = std::move(letCopy);
    }
    else if (auto call = dynamic_cast<const ASTFunctionCall*>(stmt->content.get()))
    {
        auto callCopy = std::make_unique<ASTFunctionCall>();
        callCopy->name = call->name;
        for (const auto &arg : call->args)
            callCopy->args.push_back(CloneExpression(arg.get()));
        copy->content = std::move(callCopy);
    }
    else if (auto fdef = dynamic_cast<const ASTFunctionDef*>(stmt->content.get()))
    {
        auto fCopy = std::make_unique<ASTFunctionDef>();
        fCopy->name = fdef->name;
        fCopy->parameters = fdef->parameters;
        for (const auto &s : fdef->body)
            fCopy->body.push_back(CloneStatement(s.get()));
        copy->content = std::move(fCopy);
    }
    else
    {
        throw std::runtime_error("Unknown statement type in CloneStatement");
    }

    return copy;
}
