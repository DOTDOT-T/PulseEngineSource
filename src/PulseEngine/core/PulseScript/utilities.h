#pragma once

#include <unordered_map>
#include <variant>

enum class TokenType {
    Identifier,
    Number,             // "3"
    FloatingNumber,     // "3.14"
    StringLiteral,      // a string like -> "this is a string"
    Function,           // custom function made by user

    // script syntax
    Let,                // var declaraction
    Turn,               // var modification
    Arrow,              // "->"
    LParen,             // "("
    RParen,             // ")"
    Comma,              // ","
    LBrace,             // "{"
    RBrace,             // "}"
    If,
    Else,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    EqualEqual,
    NotEqual,
    
    // arithmetic operator
    Plus,               // "+"
    Minus,              // "-"
    Star,               // "*"
    Slash,              // "/"

    // argument specifier
    Reference,          // reference to a variable
    Copy,               // copy variable content
    Const_Reference,    // reference to a variable that can't be modified
    
    EndOfFile
};

enum class ParamPassMethod
{
    COPY,
    REFERENCE,
    CONST_REFERENCE,

    NOT_A_PARAMETER
};




using Value = std::variant<int, float, std::string>;

//base struct for parser
struct ASTNode
{
    virtual ~ASTNode() = default;
};

struct Variable
{
    Value value;
    bool isGlobal = true;
    ParamPassMethod passMethod = ParamPassMethod::NOT_A_PARAMETER;
    std::string name = "USELESS";
};

struct Parameter
{
    std::string name;    
    ParamPassMethod passMethod = ParamPassMethod::COPY;
};

struct Scope {
    Scope* parent = nullptr;
    std::unordered_map<std::string, Variable> variables;

    Variable* Find(const std::string& name) {
        if (auto it = variables.find(name); it != variables.end())
            return &it->second;
        if (parent) return parent->Find(name);
        return nullptr;
    }

    void Set(const std::string& name, const Variable& var) {
        if (auto it = variables.find(name); it != variables.end())
            it->second = var;
        else if (parent && parent->Find(name))
            parent->Set(name, var);
        else
            variables[name] = var;
    }
};


struct ASTExpression : ASTNode
{
    virtual ~ASTExpression() {};
    virtual Value Evaluate(Scope& scope) const = 0;
};

