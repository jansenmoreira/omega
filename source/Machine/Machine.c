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
    free(self->global.buffer);
    free(self->stack.buffer);
}
