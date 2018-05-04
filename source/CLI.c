#include <Interpreter/Interpreter.h>
#include <Machine/Execution.h>
#include <Utility/Performance.h>

int main(int argc, char** argv)
{
    Program program[30];

    program[0].instruction = INS_ALLOC_N;
    program[1].imm_i64 = 32;
    program[2].instruction = INS_PUSH_64;
    program[3].imm_i64 = 1;
    program[4].instruction = INS_STORE_64;
    program[5].imm_i64 = 0;
    program[6].instruction = INS_LOAD_64;
    program[7].imm_i64 = 0;
    program[8].instruction = INS_DEC_I64;
    program[9].instruction = INS_STORE_64;
    program[10].imm_i64 = 8;
    program[11].instruction = INS_FREE_N;
    program[12].imm_i64 = 32;
    program[13].instruction = INS_HALT;

    Machine machine = Machine_create(1024);
    execute(program, &machine);

    return 0;
}
