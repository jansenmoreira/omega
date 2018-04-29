#ifndef STACK_H
#define STACK_H

#include <Utility/Panic.h>

typedef struct Stack
{
    u8 *buffer;
    size_t size;
    size_t capacity;
    size_t sizeof_value;
} Stack;

Stack Stack_create(size_t sizeof_value);

void Stack_destroy(Stack *self);

void Stack_clear(Stack *self);

void Stack_grow(Stack *self, size_t n);

void Stack_shrink(Stack *self, size_t n);

void Stack_push(Stack *self, void *value);

void Stack_push_stack(Stack *self, Stack *other);

#define STACK(T) Stack
#define STACK_CREATE(T) (Stack_create(sizeof(T)))
#define STACK_DESTROY(T, self) (Stack_destroy(&self))
#define STACK_CLEAR(T, self) (Stack_clear(&self))
#define STACK_GROW(T, self, n) (Stack_grow(&self, n))
#define STACK_SHRINK(T, self, n) (Stack_shrink(&self, n))
#define STACK_PUSH(T, self, value) (Stack_push(&self, &value))
#define STACK_PUSH_STACK(T, self, other) (Stack_push(&self, &other))
#define STACK_GET(T, self, index) \
    (*(T *)(self.buffer + self.sizeof_value * index))

#endif
