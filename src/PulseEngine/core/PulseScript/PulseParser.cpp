#include "PulseParser.h"
#include <stdexcept>
#include <iostream>

//
// ─── TOKEN UTILITIES ─────────────────────────────────────────────────────────
//

const Token &PulseParser::Peek(int offset) const
{
    size_t index = pos + offset;
    if (index >= tokens.size())
        return tokens.back();
    return tokens[index];
}

const Token &PulseParser::Next()
{
    if (pos < tokens.size())
        pos++;
    return Peek();
}

bool PulseParser::Match(TokenType type)
{
    if (Peek().type == type)
    {
        Next();
        return true;
    }
    return false;
}

bool PulseParser::Consume(TokenType type, const char *error)
{
    if (Peek().type != type)
    {
        throw std::runtime_error(error);
    }
    Next();
    return true;
}

// MAIN ENTRY

PulseParser::PulseParser(const std::vector<Token> &tokens)
    : tokens(tokens)
{
}

std::vector<std::unique_ptr<ASTStatement>> PulseParser::ParseScript()
{
    std::vector<std::unique_ptr<ASTStatement>> stmts;

    while (Peek().type != TokenType::EndOfFile)
    {
        stmts.push_back(ParseStatement());
    }

    return stmts;
}

// PARSE STATEMENT

std::unique_ptr<ASTStatement> PulseParser::ParseStatement()
{
    auto stmt = std::make_unique<ASTStatement>();

    if (Peek().type == TokenType::Let)
    {
        stmt->content = ParseLet();
        return stmt;
    }

    if (Peek().type == TokenType::Function)
    {
        stmt->content = ParseFunctionDef();
        return stmt;
    }

    if(Peek().type == TokenType::If)
    {
        return ParseIf();
    }

    if (Peek().type == TokenType::Identifier)
    {
        // Ne pas avancer le curseur ici
        std::string name = Peek().text;
        stmt->content = ParseFunctionCall(name);
        return stmt;
    }

    throw std::runtime_error("Unexpected token at statement start");
}

// LET x -> expr

std::unique_ptr<ASTLetStatement> PulseParser::ParseLet()
{
    Consume(TokenType::Let, "Expected 'let'");

    if (Peek().type != TokenType::Identifier)
        throw std::runtime_error("Expected identifier after 'let'");

    std::string name = Peek().text;
    Next(); // consume identifier

    Consume(TokenType::Arrow, "Expected '->' after identifier");

    auto value = ParseExpression();

    auto letStmt = std::make_unique<ASTLetStatement>();
    letStmt->varName = name;
    letStmt->value = std::move(value);

    return letStmt;
}

// EXPRESSIONS

std::unique_ptr<ASTExpression> PulseParser::ParseExpression()
{
    auto left = ParseTerm(); // existing arithmetic parsing

    while (true)
    {
        TokenType t = Peek().type;
        std::string op;
        if (t == TokenType::Greater) op = ">";       // you'll need to define these in TokenType
        else if (t == TokenType::Less) op = "<";
        else if (t == TokenType::GreaterEqual) op = ">=";
        else if (t == TokenType::LessEqual) op = "<=";
        else if (t == TokenType::EqualEqual) op = "=";
        else if (t == TokenType::NotEqual) op = "!=";
        else break;

        Next(); // consume the operator
        auto right = ParseTerm();
        left = std::make_unique<ASTBinaryComparison>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTExpression> PulseParser::ParseTerm()
{
    auto left = ParseFactor();

    while (Peek().type == TokenType::Star || Peek().type == TokenType::Slash)
    {
        char op = (Peek().type == TokenType::Star) ? '*' : '/';
        Next();
        auto right = ParseFactor();
        left = std::make_unique<ASTBinaryOp>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTExpression> PulseParser::ParseFactor()
{
    Token t = Peek();

    if (t.type == TokenType::Number)
    {
        Next();
        return std::make_unique<ASTNumber>(std::stoi(t.text));
    }
    if (t.type == TokenType::FloatingNumber)
    {
        Next();
        return std::make_unique<ASTFloatingNumber>(std::stof(t.text));
    }

    if (t.type == TokenType::StringLiteral)
    {
        Next();
        return std::make_unique<ASTString>(t.text);
    }

    if (t.type == TokenType::Identifier)
    {
        std::string name = t.text;
        if (Peek(1).type == TokenType::LParen)
        {
            return ParseFunctionCall(name);
        }
        Next();
        return std::make_unique<ASTIdentifier>(name);
    }

    if (t.type == TokenType::LParen)
    {
        Next();
        auto expr = ParseExpression();
        Consume(TokenType::RParen, "Expected ')'");
        return expr;
    }

    throw std::runtime_error("Invalid expression start");
}

std::unique_ptr<ASTExpression> PulseParser::ParsePrimary()
{
    Token t = Peek();

    if (t.type == TokenType::Number)
    {
        Next();
        return std::make_unique<ASTNumber>(std::stoi(t.text));
    }
    if (t.type == TokenType::FloatingNumber)
    {
        Next();
        return std::make_unique<ASTFloatingNumber>(std::stof(t.text));
    }

    if (t.type == TokenType::StringLiteral)
    {
        Next();
        return std::make_unique<ASTString>(t.text);
    }

    if (t.type == TokenType::Identifier)
    {
        // Ici on regarde si c'est un appel de fonction
        std::string name = t.text;

        if (Peek(1).type == TokenType::LParen)
        {
            // Ne pas consommer l'identifiant ici, laisser ParseFunctionCall gérer
            return ParseFunctionCall(name);
        }

        Next(); // simple identifiant
        return std::make_unique<ASTIdentifier>(name);
    }

    throw std::runtime_error("Invalid expression start");
}

std::unique_ptr<ASTStatement> PulseParser::ParseIf()
{
    Consume(TokenType::If, "Expected 'if'");

    auto condition = ParseExpression(); 

    Consume(TokenType::LBrace, "Expected '{' after 'if' condition");

    std::vector<std::unique_ptr<ASTStatement>> thenBranch;
    while (Peek().type != TokenType::RBrace && Peek().type != TokenType::EndOfFile)
    {
        thenBranch.push_back(ParseStatement());
    }
    Consume(TokenType::RBrace, "Expected '}' after 'if' block");

    std::vector<std::unique_ptr<ASTStatement>> elseBranch;
    if (Match(TokenType::Else))
    {
        Consume(TokenType::LBrace, "Expected '{' after 'else'");
        while (Peek().type != TokenType::RBrace && Peek().type != TokenType::EndOfFile)
        {
            elseBranch.push_back(ParseStatement());
        }
        Consume(TokenType::RBrace, "Expected '}' after 'else' block");
    }

    auto ifNode = std::make_unique<ASTIfStatement>();
    ifNode->condition = std::move(condition);
    ifNode->thenBranch = std::move(thenBranch);
    ifNode->elseBranch = std::move(elseBranch);

    auto stmt = std::make_unique<ASTStatement>();
    stmt->content = std::move(ifNode);
    return stmt;
}



// FUNCTION CALL :  name(expr, expr...)

std::unique_ptr<ASTFunctionCall> PulseParser::ParseFunctionCall(const std::string &name)
{
    auto call = std::make_unique<ASTFunctionCall>();
    call->name = name;

    Consume(TokenType::Identifier, "Expected function name"); // consomme le nom

    Consume(TokenType::LParen, "Expected '(' after function name");

    if (Peek().type != TokenType::RParen)
    {
        call->args.push_back(ParseExpression());
        while (Match(TokenType::Comma))
        {
            call->args.push_back(ParseExpression());
        }
    }

    Consume(TokenType::RParen, "Expected ')' after function arguments");

    return call;
}

std::unique_ptr<ASTFunctionDef> PulseParser::ParseFunctionDef()
{
    Consume(TokenType::Function, "Expected 'function' keyword");

    if (Peek().type != TokenType::Identifier)
        throw std::runtime_error("Expected function name after 'function'");

    std::string funcName = Peek().text;
    Next(); // consume function name

    Consume(TokenType::LParen, "Expected '(' after function name");

    // Parse parameter list
    std::vector<Parameter> params;

    if (Peek().type != TokenType::RParen)
    {
        while (true)
        {

            if (Peek().type == TokenType::Identifier)
            {
                params.push_back({.name = Peek().text, .passMethod = ParamPassMethod::COPY});
                Next(); // consume identifier
            }
            else if (Peek().type == TokenType::Copy)
            {
                params.push_back({.name = Peek(1).text, .passMethod = ParamPassMethod::COPY});
                Next(); // consume pass method
                Next(); // consume identifier
            }
            else if (Peek().type == TokenType::Reference)
            {
                params.push_back({.name = Peek(1).text, .passMethod = ParamPassMethod::REFERENCE});
                Next(); // consume pass method
                Next(); // consume identifier
            }
            else if (Peek().type == TokenType::Const_Reference)
            {
                params.push_back({.name = Peek(1).text, .passMethod = ParamPassMethod::CONST_REFERENCE});
                Next(); // consume pass method
                Next(); // consume identifier
            }


            if (Peek().type == TokenType::Comma)
            {
                Next(); // consume comma and continue
                continue;
            }
            break;
        }
    }

    Consume(TokenType::RParen, "Expected ')' after parameters");

    Consume(TokenType::LBrace, "Expected '{' to start function body");

    std::vector<std::unique_ptr<ASTStatement>> body;

    while (Peek().type != TokenType::RBrace &&
           Peek().type != TokenType::EndOfFile)
    {
        body.push_back(ParseStatement());
    }

    Consume(TokenType::RBrace, "Expected '}' at end of function");

    auto func = std::make_unique<ASTFunctionDef>();
    func->name = funcName;
    func->parameters = params;
    func->body = std::move(body);

    return func;
}
