#ifndef MACHINE_H
#define MACHINE_H

#include <Structures/Stack.h>
#include <Utility/Standard.h>

typedef struct Machine
{
    struct Machine_Stack
    {
        Value* buffer;
        size_t size;
        size_t capacity;
    } stack;

    struct Machine_Local
    {
        Value** buffer;
        size_t size;
        size_t capacity;
    } local;

    struct Machine_Global
    {
        PTR* buffer;
        size_t size;
        size_t capacity;
    } global;

    U64 cmp;
} Machine;

Machine Machine_create();

void Machine_destroy(Machine* self);

void Machine_grow_stack(Machine* self, size_t size);

void Machine_grow_local(Machine* self, size_t size);

void Machine_grow_global(Machine* self, size_t size);

#endif
