#ifndef MACHINE_H
#define MACHINE_H

#include <Structures/Stack.h>
#include <Utility/Standard.h>

typedef struct Machine
{
    u8* buffer;

    size_t capacity;
    size_t base;
    size_t size;

    u8 cmp;
} Machine;

Machine Machine_create(size_t initial_memory);

void Machine_destroy(Machine* self);

#endif
