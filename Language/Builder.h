/*
#ifndef BUILDER_H
#define BUILDER_H

#include <Language/AST.h>
#include <Language/Types.h>

typedef struct Symbol
{
    void* value;
    Type* type;
    Boolean is_const;
    Boolean has_initialized;
} Symbol;

typedef struct Scope
{
    struct Scope* parent;
    Map symbol_table;
} Scope;

typedef struct Builder
{
    struct Scopes
    {
        Scope* global;
        Scope* local;
    } scopes;

    Machine machine;
} Builder;

Builder Builder_create();

void Builder_destroy(Builder* self);

Program* Builder_build(Builder* self, AST* ast);

#endif
*/
