#include <Interpreter/Interpreter.h>

#include <Machine/Execution.h>

int main(int argc, char** argv)
{
    Program program[100];

    u64 teste = 15;

    program[0].instruction = INS_PUSH_N;
    program[1].imm_i64 = &teste;
    program[2].imm_i64 = 8ULL;
    program[3].instruction = INS_CMPI_U64;
    program[4].imm_i64 = 20ULL;
    program[5].instruction = INS_JEQ;
    program[6].imm_i64 = 11ULL;
    program[7].instruction = INS_ADDI_I64;
    program[8].imm_i64 = 1ULL;
    program[9].instruction = INS_JMP;
    program[10].imm_i64 = 3ULL;
    program[11].instruction = INS_MOV_N;
    program[12].imm_i64 = &teste;
    program[13].imm_i64 = 8ULL;
    program[14].instruction = INS_HALT;

    Machine machine = Machine_create(1024);

    execute(program, &machine);

    return 0;
}
