#ifndef PARSER_H
#define PARSER_H

#include <Language/AST.h>
#include <Language/Lexer.h>
#include <Support/Console.h>
#include <Support/Map.h>
#include <Support/Performance.h>
#include <Support/Stack.h>

typedef struct Parser
{
    Lexer lexer;
    Token token;
    Stack queue;
    size_t queue_it;
} Parser;

AST* Parse(String path);

#endif
