#include <Interpreter/Interpreter.h>
#include <Machine/Execution.h>
#include <Utility/Performance.h>

int main(int argc, char** argv)
{
    Program program[30];

    U64 ex = 0;

    program[0].instruction = INS_ALLOC;
    program[1].immediate.u64[0] = 33;
    program[2].instruction = INS_PUSH_64;
    program[3].immediate.u64[0] = 36;
    program[4].instruction = INS_STL_64;
    program[5].immediate.u64[0] = 8;

    program[6].instruction = INS_LEAL;
    program[7].immediate.u64[0] = 16;
    program[8].instruction = INS_LEAL;
    program[9].immediate.u64[0] = 0;
    program[10].instruction = INS_COPY;
    program[11].immediate.u64[0] = 16;

    program[12].instruction = INS_LDL_64;
    program[13].immediate.u64[0] = 24;
    program[14].instruction = INS_STI_64;
    program[15].immediate.ptr[0] = (PTR)(&ex);

    program[16].instruction = INS_FREE;
    program[17].instruction = INS_HALT;

    Machine machine = Machine_create(1024);
    execute(program, &machine);

    return 0;
}
