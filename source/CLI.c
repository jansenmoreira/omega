#include <Interpreter/Interpreter.h>
#include <Machine/Call.h>
#include <Machine/Execution.h>
#include <Utility/Performance.h>

int main(int argc, char** argv)
{
    Program program[30];

    program[0].instruction = INS_PUSH_64;
    program[1].imm_i64 = 0;

    program[2].instruction = INS_CMPI_U64;
    program[3].imm_i64 = 1000;
    program[4].instruction = INS_JEQ;
    program[5].imm_i64 = 24;

    program[6].instruction = INS_ALLOC_32;
    program[7].instruction = INS_PUSH_64;
    program[8].imm_i64 = (U64)("Nativéç_Call: %llu %lf\n");
    program[9].instruction = INS_PUSH_64;
    program[10].imm_i64 = 3;
    program[11].instruction = INS_PUSH_64;
    program[12].imm_fp64 = 3.3;

    program[13].instruction = INS_NATIVE_CALL;
    program[14].ptr = Print;
    program[15].imm_i32[0] = Program_Type_I32;
    program[15].imm_i32[1] = 4;
    program[16].imm_i64 = 3;
    program[17].imm_i32[0] = Program_Type_I64;
    program[17].imm_i32[1] = 8;
    program[18].imm_i32[0] = Program_Type_I64;
    program[18].imm_i32[1] = 8;
    program[19].imm_i32[0] = Program_Type_FP64;
    program[19].imm_i32[1] = 8;

    program[20].instruction = INS_POP_32;
    program[21].instruction = INS_INC_I64;

    program[22].instruction = INS_JMP;
    program[23].imm_i64 = 2;

    program[24].instruction = INS_HALT;

    Machine machine = Machine_create(1024);

    Clock_start();
    double start, end, t0, t1;

    start = Clock_get();

    execute(program, &machine);

    end = Clock_get();

    t0 = end - start;

    printf("%lf\n", t0);

    start = Clock_get();

    for (int i = 0; i < 1000; i++)
    {
        Print("Nativéç_Call: %llu %lf\n", 3ULL, 3.3);
    }

    end = Clock_get();

    t1 = end - start;

    printf("%lf\n", t1);
    printf("Ratio: %lf\n", t0 / t1);

    return 0;
}
