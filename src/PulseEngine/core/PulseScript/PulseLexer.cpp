#include "PulseLexer.h"
#include <iostream>

PulseLexer::PulseLexer(const std::string &src)
    : source(src)
{
}

bool PulseLexer::End() const
{
    return pos >= source.size();
}

void PulseLexer::SkipSpaces()
{
    while (!End() && std::isspace(source[pos]))
    {
        pos++;
    }
}

Token PulseLexer::Peek()
{
    size_t old = pos;
    Token t = Next();
    pos = old;
    return t;
}

Token PulseLexer::Next()
{
    SkipSpaces();
    if (End())
    {
        return {TokenType::EndOfFile, ""};
    }

    char c = source[pos];

    // --- punctuation ---
    if (c == '(')
    {
        pos++;
        return {TokenType::LParen, "("};
    }
    if (c == ')')
    {
        pos++;
        return {TokenType::RParen, ")"};
    }
    if (c == ',')
    {
        pos++;
        return {TokenType::Comma, ","};
    }
    if (c == '+')
    {
        pos++;
        return {TokenType::Plus, "+"};
    }
    if (c == '*')
    {
        pos++;
        return {TokenType::Star, "*"};
    }
    if (c == '/')
    {
        pos++;
        return {TokenType::Slash, "/"};
    }
    if (c == '{')
    {
        pos++;
        return {TokenType::LBrace, "{"};
    }
    if (c == '}')
    {
        pos++;
        return {TokenType::RBrace, "}"};
    }

    // --- arrow "->" ---
    if (c == '-' && pos + 1 < source.size() && source[pos + 1] == '>')
    {
        pos += 2;
        return {TokenType::Arrow, "->"};
    }
    // --- minus, not arrow ---
    if (c == '-')
    {
        pos++;
        return {TokenType::Minus, "-"};
    }

    // --- string literal ---
    if (c == '"')
    {
        return MakeString();
    }

    // --- number ---
    if (std::isdigit(c))
    {
        return MakeNumber();
    }

    // --- identifier or keyword ---
    if (std::isalpha(c) || c == '_')
    {
        return MakeIdentifierOrKeyword();
    }

    // Unknown char (no error for now)
    pos++;
    return Next();
}

Token PulseLexer::MakeIdentifierOrKeyword()
{
    size_t start = pos;

    while (!End() &&
           (std::isalnum(source[pos]) || source[pos] == '_'))
    {
        pos++;
    }

    std::string text = source.substr(start, pos - start);

    if (text == "let")
    {
        return {TokenType::Let, text};
    }

    if (text == "function")
    {
        return {TokenType::Function, text};
    }

    if (text == "ref")
    {
        return {TokenType::Reference, text};
    }

    if (text == "copy")
    {
        return {TokenType::Copy, text};
    }

    if (text == "constref")
    {
        return {TokenType::Const_Reference, text};
    }

    return {TokenType::Identifier, text};
}

Token PulseLexer::MakeNumber()
{
    size_t start = pos;
    bool hasComma = false;

    while (!End() && (std::isdigit(source[pos]) || source[pos] == '.')) // use of the same function to find int and float
    {
        if(source[pos] == '.') hasComma = true;
        pos++;
    }

    std::string text = source.substr(start, pos - start);
    return {hasComma ? TokenType::FloatingNumber : TokenType::Number, text};
}

Token PulseLexer::MakeString()
{
    pos++; // skip opening quote
    size_t start = pos;

    while (!End() && source[pos] != '"')
    {
        pos++;
    }

    std::string text = source.substr(start, pos - start);

    if (!End())
        pos++; // consume closing quote

    return {TokenType::StringLiteral, text};
}
