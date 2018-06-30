#include <Parser/AST.h>
#include <Parser/Lexer.h>
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
