#include <Interpreter/Interpreter.h>
#include <Machine/Execution.h>
#include <Utility/Performance.h>

int main(int argc, char** argv)
{
    Program program[30];

    U64 ex = 0;

    program[0].instruction = INS_PUSH_64;
    program[1].immediate.fp32[0] = -1.36;
    program[2].instruction = INS_CONV_FP32_U8;
    program[3].instruction = INS_HALT;

    Machine machine = Machine_create(1024);
    execute(program, &machine);

    return 0;
}
