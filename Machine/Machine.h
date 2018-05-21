#ifndef MACHINE_H
#define MACHINE_H

#include <Support/Stack.h>
#include <Support/Standard.h>

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

PTR Machine_alloc_global(Machine* self, size_t size);

#endif
