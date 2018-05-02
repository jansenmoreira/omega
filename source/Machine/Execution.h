#ifndef EXECUTION_H
#define EXECUTION_H

#include <Machine/Machine.h>

typedef enum Instructions
{
    INS_HALT,

    INS_PUSH_8,
    INS_PUSH_16,
    INS_PUSH_32,
    INS_PUSH_64,
    INS_PUSH_N,

    INS_POP_8,
    INS_POP_16,
    INS_POP_32,
    INS_POP_64,
    INS_POP_N,

    INS_MOV_8,
    INS_MOV_16,
    INS_MOV_32,
    INS_MOV_64,
    INS_MOV_N,

    INS_INC_I8,
    INS_INC_I16,
    INS_INC_I32,
    INS_INC_I64,

    INS_DEC_I8,
    INS_DEC_I16,
    INS_DEC_I32,
    INS_DEC_I64,

    INS_OR_I8,
    INS_OR_I16,
    INS_OR_I32,
    INS_OR_I64,

    INS_ORI_I8,
    INS_ORI_I16,
    INS_ORI_I32,
    INS_ORI_I64,

    INS_XOR_I8,
    INS_XOR_I16,
    INS_XOR_I32,
    INS_XOR_I64,

    INS_XORI_I8,
    INS_XORI_I16,
    INS_XORI_I32,
    INS_XORI_I64,

    INS_AND_I8,
    INS_AND_I16,
    INS_AND_I32,
    INS_AND_I64,

    INS_ANDI_I8,
    INS_ANDI_I16,
    INS_ANDI_I32,
    INS_ANDI_I64,

    INS_SHR_I8,
    INS_SHR_I16,
    INS_SHR_I32,
    INS_SHR_I64,

    INS_SHRI_I8,
    INS_SHRI_I16,
    INS_SHRI_I32,
    INS_SHRI_I64,

    INS_SHL_I8,
    INS_SHL_I16,
    INS_SHL_I32,
    INS_SHL_I64,

    INS_SHLI_I8,
    INS_SHLI_I16,
    INS_SHLI_I32,
    INS_SHLI_I64,

    INS_ADD_I8,
    INS_ADD_I16,
    INS_ADD_I32,
    INS_ADD_I64,
    INS_ADD_FP32,
    INS_ADD_FP64,

    INS_ADDI_I8,
    INS_ADDI_I16,
    INS_ADDI_I32,
    INS_ADDI_I64,
    INS_ADDI_FP32,
    INS_ADDI_FP64,

    INS_SUB_I8,
    INS_SUB_I16,
    INS_SUB_I32,
    INS_SUB_I64,
    INS_SUB_FP32,
    INS_SUB_FP64,

    INS_SUBI_I8,
    INS_SUBI_I16,
    INS_SUBI_I32,
    INS_SUBI_I64,
    INS_SUBI_FP32,
    INS_SUBI_FP64,

    INS_MUL_U8,
    INS_MUL_U16,
    INS_MUL_U32,
    INS_MUL_U64,
    INS_MUL_S8,
    INS_MUL_S16,
    INS_MUL_S32,
    INS_MUL_S64,
    INS_MUL_FP32,
    INS_MUL_FP64,

    INS_MULI_U8,
    INS_MULI_U16,
    INS_MULI_U32,
    INS_MULI_U64,
    INS_MULI_S8,
    INS_MULI_S16,
    INS_MULI_S32,
    INS_MULI_S64,
    INS_MULI_FP32,
    INS_MULI_FP64,

    INS_DIV_U8,
    INS_DIV_U16,
    INS_DIV_U32,
    INS_DIV_U64,
    INS_DIV_S8,
    INS_DIV_S16,
    INS_DIV_S32,
    INS_DIV_S64,
    INS_DIV_FP32,
    INS_DIV_FP64,

    INS_DIVI_U8,
    INS_DIVI_U16,
    INS_DIVI_U32,
    INS_DIVI_U64,
    INS_DIVI_S8,
    INS_DIVI_S16,
    INS_DIVI_S32,
    INS_DIVI_S64,
    INS_DIVI_FP32,
    INS_DIVI_FP64,

    INS_MOD_U8,
    INS_MOD_U16,
    INS_MOD_U32,
    INS_MOD_U64,
    INS_MOD_S8,
    INS_MOD_S16,
    INS_MOD_S32,
    INS_MOD_S64,

    INS_MODI_U8,
    INS_MODI_U16,
    INS_MODI_U32,
    INS_MODI_U64,
    INS_MODI_S8,
    INS_MODI_S16,
    INS_MODI_S32,
    INS_MODI_S64,

    INS_CMP_U8,
    INS_CMP_U16,
    INS_CMP_U32,
    INS_CMP_U64,
    INS_CMP_S8,
    INS_CMP_S16,
    INS_CMP_S32,
    INS_CMP_S64,
    INS_CMP_FP32,
    INS_CMP_FP64,

    INS_CMPI_U8,
    INS_CMPI_U16,
    INS_CMPI_U32,
    INS_CMPI_U64,
    INS_CMPI_S8,
    INS_CMPI_S16,
    INS_CMPI_S32,
    INS_CMPI_S64,
    INS_CMPI_FP32,
    INS_CMPI_FP64,

    INS_JEQ,
    INS_JNE,
    INS_JLT,
    INS_JLE,
    INS_JGT,
    INS_JGE,
    INS_JMP,
} Instructions;

typedef union Program {
    Instructions instruction;

    u64 imm_i64;
    u32 imm_i32[2];
    u16 imm_i16[4];
    u8 imm_i8[8];
    fp64 imm_fp64;
    fp32 imm_fp32[2];

} Program;

void execute(Program* program, Machine* machine);

#endif
