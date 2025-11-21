/**
 * @file PulseLexer.h
 * @author Dorian LEXTERIAQUE (dlexteriaque@gmail.com)
 * @brief Core lexical analyzer for the PulseScript language.
 *
 * This lexer converts raw script text into a structured token stream consumed by the parser.
 * It handles:
 *   - Numeric literals (integer + floating-point)
 *   - String literals with escape support
 *   - Identifiers and keywords
 *   - Operators and punctuation
 *   - Function syntax and custom language constructs
 *
 * PulseScript is a lightweight, high-level scripting language designed
 * specifically for real-time game logic inside the Pulse Engine.
 * Performance, deterministic behavior, and clean syntax are the core pillars.
 *
 * The lexer is intentionally strict: it performs early validation and raises
 * descriptive errors to ensure the parser only receives fully valid tokens.
 *
 * @version 0.2
 * @date 2025-11-20
 *
 * @copyright
 * Copyright (c) 2025 Pulse Engine
 * All rights reserved.
 *
 */


#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <variant>

#include "utilities.h"


struct Token
{
    TokenType type;
    std::string text;
};

class PulseLexer
{
public:
    PulseLexer(const std::string &src);

    Token Next();
    Token Peek();
    bool End() const;

private:
    void SkipSpaces();
    Token MakeIdentifierOrKeyword();
    Token MakeNumber();
    Token MakeString();

private:
    std::string source;
    size_t pos = 0;
};
