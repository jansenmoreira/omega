#ifndef LEXER_H
#define LEXER_H

#include <Structures/Map.h>
#include <Structures/Stack.h>
#include <Utility/String.h>
#include <Utility/Unicode.h>

enum Tag
{
    Tag_END = -1,

    Tag_ID = 256,

    Tag_LITERAL_INTEGER,
    Tag_LITERAL_REAL,
    Tag_LITERAL_STRING,
    Tag_LITERAL_CHAR,

    Tag_ARROW,

    Tag_LSHIFT,
    Tag_RSHIFT,

    Tag_GE,
    Tag_LE,
    Tag_EQ,
    Tag_NE,

    Tag_IF,
    Tag_ELSE,
    Tag_WHILE,
    Tag_RETURN,
    Tag_CONTINUE,
    Tag_BREAK,
    Tag_VAR,
    Tag_CONST,
    Tag_TYPE,
    Tag_USE,
    Tag_AS,
    Tag_IMPORT,
    Tag_EXPORT,
    Tag_VOID,
    Tag_S_CAST,
    Tag_S_SIZE,

    Tag_UNKNOW,
};

typedef struct Token
{
    int tag;
    String lexeme;
    size_t begin;
    size_t end;
    String path;
} Token;

typedef struct Lexer
{
    const char* buffer;
    size_t at;
    int ignore;
    String path;
    Stack lexeme;
} Lexer;

void Lexer_init();

void Lexer_free();

Lexer Lexer_create(const char* buffer, String path);

void Lexer_destroy(Lexer* self);

Token Lexer_next(Lexer* self);

#endif