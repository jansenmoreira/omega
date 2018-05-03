#include <Interpreter/Interpreter.h>
#include <Machine/Call.h>
#include <Machine/Execution.h>
#include <Utility/Performance.h>

int main(int argc, char** argv)
{
    Call_Value v;
    v.ptr = NULL;

    size_t t = Native_Call(Call_Integral_1, v, 0, NULL);

    /*
    Program program[30];

    program[0].instruction = INS_PUSH_64;
    program[1].imm_i64 = 0;
    program[2].instruction = INS_CMPI_U64;
    program[3].imm_i64 = 10;
    program[4].instruction = INS_JEQ;
    program[5].imm_i64 = 24;
    program[6].instruction = INS_ALLOC_32;
    program[7].instruction = INS_PUSH_64;
    program[8].imm_i64 = (u64)("Nativéç_Call: %d %lf\n");
    program[9].instruction = INS_PUSH_64;
    program[10].imm_i64 = 3;
    program[11].instruction = INS_PUSH_64;
    program[12].imm_fp64 = 3.3;
    program[13].instruction = INS_NATIVE_CALL;
    program[14].ptr = Print;
    program[15].imm_i32[0] = Native_Call_Integer;
    program[15].imm_i32[1] = 4;
    program[16].imm_i64 = 3;
    program[17].imm_i32[0] = Native_Call_Integer;
    program[17].imm_i32[1] = 8;
    program[18].imm_i32[0] = Native_Call_Integer;
    program[18].imm_i32[1] = 8;
    program[19].imm_i32[0] = Native_Call_Float;
    program[19].imm_i32[1] = 8;
    program[20].instruction = INS_POP_32;
    program[21].instruction = INS_INC_I64;
    program[22].instruction = INS_JMP;
    program[23].imm_i64 = 2;
    program[24].instruction = INS_HALT;

    Machine machine = Machine_create(1024);
    execute(program, &machine);

    u64 i, n, step, it;

    scanf("%llu%llu%llu", &i, &n, &step);

    program[0].instruction = INS_PUSH_64;
    program[1].imm_i64 = i;
    program[2].instruction = INS_CMPI_U64;
    program[3].imm_i64 = n;
    program[4].instruction = INS_JEQ;
    program[5].imm_i64 = 9ULL;
    program[6].instruction = INS_INC_I64;
    program[7].instruction = INS_JMP;
    program[8].imm_i64 = 2ULL;
    program[9].instruction = INS_MOV_64;
    program[10].imm_i64 = (u64)&it;
    program[11].instruction = INS_HALT;

    Machine machine = Machine_create(1024);

    Clock_start();
    double start, end, t0, t1;

    start = Clock_get();

    execute(program, &machine);

    end = Clock_get();

    t0 = end - start;

    printf("%lf %" PRIu64 "\n", t0, it);

    start = Clock_get();

    it = i;

    while (it < n)
    {
        it += step;
    }

    end = Clock_get();

    t1 = end - start;

    printf("%lf %" PRIu64 "\n", t1, it);
    printf("Ratio: %lf\n", t0 / t1);
    */

    return 0;
}
