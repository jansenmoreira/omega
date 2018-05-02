/*
#ifndef MACHINE_H
#define MACHINE_H

#include <Interpreter/Expression.h>
#include <Interpreter/Lexer.h>
#include <Interpreter/Types.h>
#include <Structures/Map.h>
#include <Structures/Stack.h>

typedef struct Scope
{
    struct Scope* parent;
    MAP(size_t) table;
    STACK(u8) values;
} Scope;

typedef struct Machine
{
    Scope* global;
    Scope* actual;

    STACK(u8) stack;
    MAP(Type*) types;
} Machine;

Machine Machine_create();
void Machine_print_top(Machine* self);
void Machine_scope_push(Machine* self);
void Machine_scope_pop(Machine* self);
void Machine_scope_add(Machine* self, String id, Type* type, void* value);
Boolean Machine_evaluate(Machine* self, Expression* expression);

#endif
* /
