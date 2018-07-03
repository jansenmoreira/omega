#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <Language/AST.h>
#include <Language/Types.h>
#include <Machine/Execution.h>

typedef struct Symbol
{
    void* storage;
    Type* type;
    Boolean constant;
    Boolean initialized;
} Symbol;

typedef struct Scope
{
    struct Scope* parent;
    Map symbol_table;
} Scope;

typedef struct Interpreter
{
    struct Scopes
    {
        Scope* global;
        Scope* local;
    } scopes;

    Machine machine;
} Interpreter;

Interpreter Interpreter_create();
void Interpreter_run(Interpreter* self, AST* ast);

// Program* Builder_build(Builder* self, AST* ast);

#endif
