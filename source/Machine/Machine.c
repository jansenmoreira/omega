#include <Machine/Machine.h>

Machine Machine_create(size_t initial_memory)
{
    Machine self;

    self.stack = (U8*)malloc(sizeof(U8) * initial_memory);
    self.base = 0;
    self.size = 0;
    self.capacity = initial_memory;

    self.cmp = 0;

    self.local_memory = Stack_create(sizeof(U8*));
    self.global_memory = Stack_create(sizeof(U8*));

    return self;
}

void Machine_alloc(Machine* self, size_t size)
{
    if (self->size + size > self->capacity)
    {
        size_t capacity = (self->size + size) << 1;
        U8* buffer = (U8*)malloc(sizeof(U8) * capacity);

        if (!buffer)
        {
            Panic(Memory_Error);
        }

        memcpy(buffer, self->stack, self->size);

        self->stack = buffer;
        self->capacity = capacity;
    }
}

void Machine_destroy(Machine* self)
{
    Stack_destroy(&self->local_memory);
    Stack_destroy(&self->global_memory);
    free(self->stack);
}
