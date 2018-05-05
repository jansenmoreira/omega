#include <Machine/Machine.h>

Machine Machine_create()
{
    Machine self;

    self.stack.buffer = (Value*)malloc(sizeof(Value) * 1);
    self.stack.capacity = 1;
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
    free(self->global.buffer);
    free(self->stack.buffer);
}

void Machine_grow_stack(Machine* self, size_t size)
{
    if (self->stack.size + size > self->stack.capacity)
    {
        size_t capacity = (self->stack.size + size) << 1;
        Value* buffer = (Value*)malloc(sizeof(Value) * capacity);

        if (!buffer)
        {
            Panic(Memory_Error);
        }

        memcpy(buffer, self->stack.buffer, sizeof(Value) * self->stack.size);

        self->stack.buffer = buffer;
        self->stack.capacity = capacity;
    }
}

void Machine_grow_local(Machine* self, size_t size)
{
    if (self->local.size + size > self->local.capacity)
    {
        size_t capacity = (self->local.size + size) << 1;
        Value** buffer = (Value**)malloc(sizeof(Value*) * capacity);

        if (!buffer)
        {
            Panic(Memory_Error);
        }

        memcpy(buffer, self->local.buffer, sizeof(Value*) * self->local.size);

        self->local.buffer = buffer;
        self->local.capacity = capacity;
    }
}

void Machine_grow_global(Machine* self, size_t size)
{
    if (self->global.size + size > self->global.capacity)
    {
        size_t capacity = (self->global.size + size) << 1;
        PTR* buffer = (PTR*)malloc(sizeof(PTR) * capacity);

        if (!buffer)
        {
            Panic(Memory_Error);
        }

        memcpy(buffer, self->global.buffer, sizeof(PTR) * self->global.size);

        self->global.buffer = buffer;
        self->global.capacity = capacity;
    }
}
