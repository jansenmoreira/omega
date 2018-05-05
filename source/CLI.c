#include <Interpreter/Interpreter.h>
#include <Machine/Execution.h>
#include <Utility/Performance.h>

int main(int argc, char** argv)
{
    Program program[30];

    U64 ex = 0;

    program[0].instruction = INS_ALLOC;
    program[1].imm_i64 = 32;
    program[2].instruction = INS_PUSH_64;
    program[3].imm_i64 = 45;
    program[4].instruction = INS_STL_64;
    program[5].imm_i64 = 0;
    program[6].instruction = INS_LDL_64;
    program[7].imm_i64 = 0;
    program[8].instruction = INS_STI_64;
    program[9].ptr = &ex;
    program[10].instruction = INS_FREE;
    program[11].instruction = INS_HALT;

    Machine machine = Machine_create(1024);
    execute(program, &machine);

    return 0;
}
