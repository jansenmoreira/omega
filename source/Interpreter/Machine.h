#ifndef MACHINE_H
#define MACHINE_H

#include <Interpreter/Expression.h>
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

#endif
