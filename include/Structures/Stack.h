#ifndef STACK_H
#define STACK_H

#include <Utility/Panic.h>

typedef struct Stack
{
    U8* buffer;
    size_t size;
    size_t capacity;
    size_t sizeof_value;
} Stack;

Stack Stack_create(size_t sizeof_value);

void Stack_destroy(Stack* self);

void Stack_clear(Stack* self);

void Stack_grow(Stack* self, size_t n);

void Stack_shrink(Stack* self, size_t n);

void Stack_push(Stack* self, void* value);

void Stack_push_stack(Stack* self, Stack* other);

void* Stack_get(Stack* self, size_t index);

#endif
