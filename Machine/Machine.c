#include <Machine/Machine.h>

Machine Machine_create()
{
    Machine self;

    self.stack.buffer = (Value*)malloc(sizeof(Value) * 1024);
    self.stack.capacity = 1024;
    self.stack.size = 0;

    self.local.buffer = (Value**)malloc(sizeof(Value) * 1);
    self.local.capacity = 1;
    self.local.size = 0;

    self.global.buffer = (PTR*)malloc(sizeof(Value) * 1);
    self.global.capacity = 1;
    self.global.size = 0;

    if (!self.stack.buffer || !self.local.buffer || !self.global.buffer)
    {
        Panic(Memory_Error);
    }

    return self;
}

void Machine_destroy(Machine* self)
{
    free(self->local.buffer);
    free(self->stack.buffer);

    for (size_t i = 0; i < self->global.size; i++)
    {
        free(self->global.buffer[i]);
    }

    free(self->global.buffer);
}

PTR Machine_alloc_global(Machine* self, size_t size)
{
    PTR* global_buffer = malloc(size);

    if (!global_buffer)
    {
        Panic(Memory_Error);
    }

    if (self->global.size + 1 > self->global.capacity)
    {
        size_t capacity = (self->global.size + 1) << 1;
        PTR* buffer = (PTR*)malloc(sizeof(PTR) * capacity);

        if (!buffer)
        {
            Panic(Memory_Error);
        }

        memcpy(buffer, self->global.buffer, sizeof(PTR) * self->global.size);

        self->global.buffer = buffer;
        self->global.capacity = capacity;
    }

    self->global.buffer[self->global.size++] = global_buffer;
    return global_buffer;
}
