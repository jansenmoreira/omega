#include <Machine/Machine.h>

Machine Machine_create(size_t initial_memory)
{
    Machine self;

    self.buffer = (U8*)malloc(sizeof(U8) * initial_memory);
    self.base = 0;
    self.size = 0;
    self.capacity = initial_memory;

    self.cmp = 0;

    return self;
}

void Machine_destroy(Machine* self)
{
    free(self->buffer);
}
