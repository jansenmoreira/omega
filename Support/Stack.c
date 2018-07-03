#include <Support/Stack.h>

Stack Stack_create(size_t sizeof_value)
{
    Stack self;
    self.sizeof_value = sizeof_value;
    self.buffer = (U8*)malloc(self.sizeof_value * 8);

    if (!self.buffer)
        Panic(Memory_Error);

    self.size = 0;
    self.capacity = 8;
    return self;
}

void Stack_destroy(Stack* self)
{
    free(self->buffer);
    self->buffer = NULL;
}

void Stack_clear(Stack* self)
{
    self->size = 0;
}

static void Stack_reserve(Stack* self, size_t n)
{
    if (n <= self->capacity)
        return;

    U8* tmp_buffer = (U8*)malloc(self->sizeof_value * n);

    if (!tmp_buffer)
        Panic(Memory_Error);

    memcpy(tmp_buffer, self->buffer, self->sizeof_value * self->size);
    free(self->buffer);
    self->buffer = tmp_buffer;
    self->capacity = n;
}

static void Stack_resize(Stack* self, size_t n)
{
    if (n <= self->capacity)
    {
        self->size = n;
        return;
    };

    Stack_reserve(self, n << 1);
    self->size = n;
}

void Stack_grow(Stack* self, size_t n)
{
    Stack_resize(self, self->size + n);
}

void Stack_shrink(Stack* self, size_t n)
{
    Stack_resize(self, self->size - n);
}

void Stack_push(Stack* self, void* value)
{
    if (self->size == self->capacity)
    {
        Stack_reserve(self, self->capacity << 1);
    }

    memcpy(self->buffer + self->sizeof_value * self->size, value,
           self->sizeof_value);
    self->size += 1;
}

void Stack_push_stack(Stack* self, Stack* other)
{
    if (self->size + other->size >= self->capacity)
    {
        Stack_reserve(self, (self->size + other->size) << 1);
    }

    memcpy(self->buffer + self->sizeof_value * self->size, other->buffer,
           self->sizeof_value * other->size);
    self->size += other->size;
}

void* Stack_get(Stack* self, size_t index)
{
    return self->buffer + (self->sizeof_value * index);
}
