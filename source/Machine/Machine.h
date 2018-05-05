#ifndef MACHINE_H
#define MACHINE_H

#include <Structures/Stack.h>
#include <Utility/Standard.h>

typedef struct Machine
{
    U8* stack;

    size_t capacity;
    size_t base;
    size_t size;

    U8 cmp;

    Stack local_memory;
    Stack global_memory;
} Machine;

Machine Machine_create(size_t initial_memory);

void Machine_destroy(Machine* self);

void Machine_alloc(Machine* self, size_t size);

#endif
