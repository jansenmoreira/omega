#include <Interpreter/Interpreter.h>
#include <Machine/Execution.h>
#include <Utility/Performance.h>

int main(int argc, char** argv)
{
    Machine machine = Machine_create();

    double start, end;

    Clock_start();

    Program program[] = {
        {.instruction = INS_ALLOC},    {.immediate.u64 = 16},
        {.instruction = INS_PUSH_64},  {.immediate.u64 = 0},
        {.instruction = INS_PUSH_64},  {.immediate.u64 = 0},
        {.instruction = INS_STL_64},   {.immediate.u64 = 0},
        {.instruction = INS_PUSH_64},  {.immediate.u64 = 1},
        {.instruction = INS_STL_64},   {.immediate.u64 = 8},
        {.instruction = INS_LDL_64},   {.immediate.u64 = 8},
        {.instruction = INS_LDL_64},   {.immediate.u64 = 0},
        {.instruction = INS_LDL_64},   {.immediate.u64 = 8},
        {.instruction = INS_ADD_I64},  {.instruction = INS_STL_64},
        {.immediate.u64 = 8},          {.instruction = INS_STL_64},
        {.immediate.u64 = 0},          {.instruction = INS_INC_I64},
        {.instruction = INS_CMPI_U64}, {.immediate.u64 = 1000000000},
        {.instruction = INS_JNE},      {.immediate.u64 = 12},
        {.instruction = INS_HALT},
    };

    start = Clock_get();
    execute(program, &machine);
    end = Clock_get();

    printf("%lf %llu\n", end - start, machine.local.buffer[0][1].u64[0]);

    return 0;
}
