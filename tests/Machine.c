#include <Machine/Execution.h>

Boolean Test__INS_PUSH_POP()
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

    Boolean test = True;

    test &= machine.stack.buffer[0].u8[0] == 128;
    test &= machine.stack.buffer[1].u16[0] == 128;
    test &= machine.stack.buffer[2].u32[0] == 128;
    test &= machine.stack.buffer[3].u64[0] == 128;

    program[0].instruction = INS_POP;
    program[1].instruction = INS_POP;
    program[2].instruction = INS_POP;
    program[3].instruction = INS_POP;
    program[4].instruction = INS_HALT;

    execute(program, &machine);

    test &= machine.stack.size == 0;

    Print("INS_PUSH_POP: %s:\n", test ? "SUCESS" : "FAILED");

    Machine_destroy(&machine);

    return test;
}

Boolean Test__INS_SGNEXT_ZEREXT()
{
    Machine machine = Machine_create();
    Program program[37];

    program[0].instruction = INS_PUSH_8;
    program[1].immediate.u8[0] = -1;
    program[2].instruction = INS_SGNEXT_I8_I16;

    program[3].instruction = INS_PUSH_8;
    program[4].immediate.u8[0] = -1;
    program[5].instruction = INS_SGNEXT_I8_I32;

    program[6].instruction = INS_PUSH_8;
    program[7].immediate.u8[0] = -1;
    program[8].instruction = INS_SGNEXT_I8_I64;

    program[9].instruction = INS_PUSH_16;
    program[10].immediate.u16[0] = -1;
    program[11].instruction = INS_SGNEXT_I16_I32;

    program[12].instruction = INS_PUSH_16;
    program[13].immediate.u16[0] = -1;
    program[14].instruction = INS_SGNEXT_I16_I64;

    program[15].instruction = INS_PUSH_32;
    program[16].immediate.u32[0] = -1;
    program[17].instruction = INS_SGNEXT_I32_I64;

    program[18].instruction = INS_PUSH_8;
    program[19].immediate.u8[0] = -1;
    program[20].instruction = INS_ZEREXT_I8_I16;

    program[21].instruction = INS_PUSH_8;
    program[22].immediate.u8[0] = -1;
    program[23].instruction = INS_ZEREXT_I8_I32;

    program[24].instruction = INS_PUSH_8;
    program[25].immediate.u8[0] = -1;
    program[26].instruction = INS_ZEREXT_I8_I64;

    program[27].instruction = INS_PUSH_16;
    program[28].immediate.u16[0] = -1;
    program[29].instruction = INS_ZEREXT_I16_I32;

    program[30].instruction = INS_PUSH_16;
    program[31].immediate.u16[0] = -1;
    program[32].instruction = INS_ZEREXT_I16_I64;

    program[33].instruction = INS_PUSH_32;
    program[34].immediate.u32[0] = -1;
    program[35].instruction = INS_ZEREXT_I32_I64;

    program[36].instruction = INS_HALT;

    execute(program, &machine);

    Boolean test = True;

    test &= machine.stack.buffer[0].s16[0] == -1;
    test &= machine.stack.buffer[1].s32[0] == -1;
    test &= machine.stack.buffer[2].s64[0] == -1;
    test &= machine.stack.buffer[3].s32[0] == -1;
    test &= machine.stack.buffer[4].s64[0] == -1;
    test &= machine.stack.buffer[5].s64[0] == -1;
    test &= machine.stack.buffer[6].u16[0] == 255;
    test &= machine.stack.buffer[7].u32[0] == 255;
    test &= machine.stack.buffer[8].u64[0] == 255;
    test &= machine.stack.buffer[9].u32[0] == 65535;
    test &= machine.stack.buffer[10].u64[0] == 65535;
    test &= machine.stack.buffer[11].u64[0] == 4294967295;

    Print("INS_SIGNEXT_ZEREXT: %s:\n", test ? "SUCESS" : "FAILED");

    Machine_destroy(&machine);

    return test;
}

Boolean Test__INS_ADD()
{
    Machine machine = Machine_create();
    Program program[55];

    program[0].instruction = INS_PUSH_8;
    program[1].immediate.u8[0] = 1;
    program[2].instruction = INS_PUSH_8;
    program[3].immediate.u8[0] = 1;
    program[4].instruction = INS_ADD_I8;

    program[5].instruction = INS_PUSH_16;
    program[6].immediate.u16[0] = 1;
    program[7].instruction = INS_PUSH_16;
    program[8].immediate.u16[0] = 1;
    program[9].instruction = INS_ADD_I16;

    program[10].instruction = INS_PUSH_32;
    program[11].immediate.u32[0] = 1;
    program[12].instruction = INS_PUSH_32;
    program[13].immediate.u32[0] = 1;
    program[14].instruction = INS_ADD_I32;

    program[15].instruction = INS_PUSH_64;
    program[16].immediate.u64[0] = 1;
    program[17].instruction = INS_PUSH_64;
    program[18].immediate.u64[0] = 1;
    program[19].instruction = INS_ADD_I64;

    program[20].instruction = INS_PUSH_32;
    program[21].immediate.fp32[0] = 1.0f;
    program[22].instruction = INS_PUSH_32;
    program[23].immediate.fp32[0] = 1.0f;
    program[24].instruction = INS_ADD_FP32;

    program[25].instruction = INS_PUSH_64;
    program[26].immediate.fp64[0] = 1.0;
    program[27].instruction = INS_PUSH_64;
    program[28].immediate.fp64[0] = 1.0;
    program[29].instruction = INS_ADD_FP64;

    program[30].instruction = INS_PUSH_8;
    program[31].immediate.u8[0] = 1;
    program[32].instruction = INS_ADDI_I8;
    program[33].immediate.u8[0] = 1;

    program[34].instruction = INS_PUSH_16;
    program[35].immediate.u16[0] = 1;
    program[36].instruction = INS_ADDI_I16;
    program[37].immediate.u16[0] = 1;

    program[38].instruction = INS_PUSH_32;
    program[39].immediate.u32[0] = 1;
    program[40].instruction = INS_ADDI_I32;
    program[41].immediate.u32[0] = 1;

    program[42].instruction = INS_PUSH_64;
    program[43].immediate.u64[0] = 1;
    program[44].instruction = INS_ADDI_I64;
    program[45].immediate.u64[0] = 1;

    program[46].instruction = INS_PUSH_32;
    program[47].immediate.fp32[0] = 1.0f;
    program[48].instruction = INS_ADDI_FP32;
    program[49].immediate.fp32[0] = 1.0f;

    program[50].instruction = INS_PUSH_64;
    program[51].immediate.fp64[0] = 1.0;
    program[52].instruction = INS_ADDI_FP64;
    program[53].immediate.fp64[0] = 1.0;

    program[54].instruction = INS_HALT;

    execute(program, &machine);

    Boolean test = True;

    test &= machine.stack.buffer[0].u8[0] == 2;
    test &= machine.stack.buffer[1].u16[0] == 2;
    test &= machine.stack.buffer[2].u32[0] == 2;
    test &= machine.stack.buffer[3].u64[0] == 2;
    test &= machine.stack.buffer[4].fp32[0] == 2.0f;
    test &= machine.stack.buffer[5].fp64[0] == 2.0;
    test &= machine.stack.buffer[6].u8[0] == 2;
    test &= machine.stack.buffer[7].u16[0] == 2;
    test &= machine.stack.buffer[8].u32[0] == 2;
    test &= machine.stack.buffer[9].u64[0] == 2;
    test &= machine.stack.buffer[10].fp32[0] == 2.0f;
    test &= machine.stack.buffer[11].fp64[0] == 2.0;

    Print("INS_ADD: %s:\n", test ? "SUCESS" : "FAILED");

    Machine_destroy(&machine);

    return test;
}

Boolean Test__INS_SUB()
{
    Machine machine = Machine_create();
    Program program[55];

    program[0].instruction = INS_PUSH_8;
    program[1].immediate.u8[0] = 1;
    program[2].instruction = INS_PUSH_8;
    program[3].immediate.u8[0] = 1;
    program[4].instruction = INS_SUB_I8;

    program[5].instruction = INS_PUSH_16;
    program[6].immediate.u16[0] = 1;
    program[7].instruction = INS_PUSH_16;
    program[8].immediate.u16[0] = 1;
    program[9].instruction = INS_SUB_I16;

    program[10].instruction = INS_PUSH_32;
    program[11].immediate.u32[0] = 1;
    program[12].instruction = INS_PUSH_32;
    program[13].immediate.u32[0] = 1;
    program[14].instruction = INS_SUB_I32;

    program[15].instruction = INS_PUSH_64;
    program[16].immediate.u64[0] = 1;
    program[17].instruction = INS_PUSH_64;
    program[18].immediate.u64[0] = 1;
    program[19].instruction = INS_SUB_I64;

    program[20].instruction = INS_PUSH_32;
    program[21].immediate.fp32[0] = 1.0f;
    program[22].instruction = INS_PUSH_32;
    program[23].immediate.fp32[0] = 1.0f;
    program[24].instruction = INS_SUB_FP32;

    program[25].instruction = INS_PUSH_64;
    program[26].immediate.fp64[0] = 1.0;
    program[27].instruction = INS_PUSH_64;
    program[28].immediate.fp64[0] = 1.0;
    program[29].instruction = INS_SUB_FP64;

    program[30].instruction = INS_PUSH_8;
    program[31].immediate.u8[0] = 1;
    program[32].instruction = INS_SUBI_I8;
    program[33].immediate.u8[0] = 1;

    program[34].instruction = INS_PUSH_16;
    program[35].immediate.u16[0] = 1;
    program[36].instruction = INS_SUBI_I16;
    program[37].immediate.u16[0] = 1;

    program[38].instruction = INS_PUSH_32;
    program[39].immediate.u32[0] = 1;
    program[40].instruction = INS_SUBI_I32;
    program[41].immediate.u32[0] = 1;

    program[42].instruction = INS_PUSH_64;
    program[43].immediate.u64[0] = 1;
    program[44].instruction = INS_SUBI_I64;
    program[45].immediate.u64[0] = 1;

    program[46].instruction = INS_PUSH_32;
    program[47].immediate.fp32[0] = 1.0f;
    program[48].instruction = INS_SUBI_FP32;
    program[49].immediate.fp32[0] = 1.0f;

    program[50].instruction = INS_PUSH_64;
    program[51].immediate.fp64[0] = 1.0;
    program[52].instruction = INS_SUBI_FP64;
    program[53].immediate.fp64[0] = 1.0;

    program[54].instruction = INS_HALT;

    execute(program, &machine);

    Boolean test = True;

    test &= machine.stack.buffer[0].u8[0] == 0;
    test &= machine.stack.buffer[1].u16[0] == 0;
    test &= machine.stack.buffer[2].u32[0] == 0;
    test &= machine.stack.buffer[3].u64[0] == 0;
    test &= machine.stack.buffer[4].fp32[0] == 0.0f;
    test &= machine.stack.buffer[5].fp64[0] == 0.0;
    test &= machine.stack.buffer[6].u8[0] == 0;
    test &= machine.stack.buffer[7].u16[0] == 0;
    test &= machine.stack.buffer[8].u32[0] == 0;
    test &= machine.stack.buffer[9].u64[0] == 0;
    test &= machine.stack.buffer[10].fp32[0] == 0.0f;
    test &= machine.stack.buffer[11].fp64[0] == 0.0;

    Print("INS_SUB: %s:\n", test ? "SUCESS" : "FAILED");

    Machine_destroy(&machine);

    return test;
}

int main()
{
    size_t errors = 0;

    errors += !Test__INS_PUSH_POP();
    errors += !Test__INS_SGNEXT_ZEREXT();
    errors += !Test__INS_ADD();
    errors += !Test__INS_SUB();

    return errors;
}
