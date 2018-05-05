#include <Machine/Execution.h>

Boolean Test__Machine_grow_stack()
{
    Machine machine = Machine_create();
    Machine_grow_stack(&machine, 2048);

    Boolean test = machine.stack.size == 0 && machine.stack.capacity == 4096;

    Print("Machine_grow_stack: %s:\n", test ? "SUCESS" : "FAILED");

    Machine_destroy(&machine);

    return test;
}

Boolean Test__INS_PUSH()
{
    Machine machine = Machine_create();
    Program program[9];

    program[0].instruction = INS_PUSH_8;
    program[1].immediate.u8[0] = 128;
    program[2].instruction = INS_PUSH_16;
    program[3].immediate.u16[0] = 128;
    program[4].instruction = INS_PUSH_32;
    program[5].immediate.u32[0] = 128;
    program[6].instruction = INS_PUSH_64;
    program[7].immediate.u64[0] = 128;
    program[8].instruction = INS_HALT;

    execute(program, &machine);

    Boolean test0 = machine.stack.buffer[0].u8[0] == 128;
    Boolean test1 = machine.stack.buffer[1].u16[0] == 128;
    Boolean test2 = machine.stack.buffer[2].u32[0] == 128;
    Boolean test3 = machine.stack.buffer[3].u64[0] == 128;

    Boolean test = test0 && test1 && test2 && test3;

    Print("INS_PUSH: %s:\n", test ? "SUCESS" : "FAILED");

    Machine_destroy(&machine);

    return test;
}

int main()
{
    size_t errors = 0;

    errors += Test__Machine_grow_stack();
    errors += Test__INS_PUSH();

    return errors;
}
