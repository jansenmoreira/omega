#include <Machine/Execution.h>

void execute(Program* program, Machine* machine)
{
    size_t counter = 0;

    for (Boolean step = True; step;)
    {
        switch (program[counter].instruction)
        {
            case INS_HALT:
            {
                step = False;
                break;
            }

            case INS_ALLOC_8:
            {
                Machine_alloc(machine, 1);
                machine->size += 1;
                counter += 1;
                break;
            }
            case INS_ALLOC_16:
            {
                Machine_alloc(machine, 2);
                machine->size += 2;
                counter += 1;
                break;
            }
            case INS_ALLOC_32:
            {
                Machine_alloc(machine, 4);
                machine->size += 4;
                counter += 1;
                break;
            }
            case INS_ALLOC_64:
            {
                Machine_alloc(machine, 8);
                machine->size += 8;
                counter += 1;
                break;
            }
            case INS_ALLOC_N:
            {
                U64 size = (program + counter + 1)->imm_i64;
                Machine_alloc(machine, size);
                machine->size += size;
                counter += 2;
                break;
            }

            case INS_PUSH_8:
            {
                Machine_alloc(machine, 1);

                U8 value = (program + counter + 1)->imm_i8[0];
                *(U8*)(machine->buffer + machine->size) = value;

                machine->size += 1;
                counter += 2;
                break;
            }
            case INS_PUSH_16:
            {
                Machine_alloc(machine, 2);

                U16 value = (program + counter + 1)->imm_i16[0];
                *(U16*)(machine->buffer + machine->size) = value;

                machine->size += 2;
                counter += 2;
                break;
            }
            case INS_PUSH_32:
            {
                Machine_alloc(machine, 4);

                U32 value = (program + counter + 1)->imm_i32[0];
                *(U32*)(machine->buffer + machine->size) = value;

                machine->size += 4;
                counter += 2;
                break;
            }
            case INS_PUSH_64:
            {
                Machine_alloc(machine, 8);

                U64 value = (program + counter + 1)->imm_i64;
                *(U64*)(machine->buffer + machine->size) = value;

                machine->size += 8;
                counter += 2;
                break;
            }

            case INS_FREE_8:
            {
                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_FREE_16:
            {
                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_FREE_32:
            {
                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_FREE_64:
            {
                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_FREE_N:
            {
                machine->size -= (program + counter + 1)->imm_i64;
                counter += 2;
                break;
            }

            case INS_LOAD_8:
            {
                size_t address = (program + counter + 1)->imm_i64;
                Machine_alloc(machine, 1);

                *(U8*)(machine->buffer + machine->size) =
                    *(U8*)(machine->buffer + address);

                machine->size += 1;
                counter += 2;
                break;
            }
            case INS_LOAD_16:
            {
                size_t address = (program + counter + 1)->imm_i64;
                Machine_alloc(machine, 2);

                *(U16*)(machine->buffer + machine->size) =
                    *(U16*)(machine->buffer + address);

                machine->size += 2;
                counter += 2;
                break;
            }
            case INS_LOAD_32:
            {
                size_t address = (program + counter + 1)->imm_i64;
                Machine_alloc(machine, 4);

                *(U32*)(machine->buffer + machine->size) =
                    *(U32*)(machine->buffer + address);

                machine->size += 4;
                counter += 2;
                break;
            }
            case INS_LOAD_64:
            {
                size_t address = (program + counter + 1)->imm_i64;
                Machine_alloc(machine, 8);

                *(U64*)(machine->buffer + machine->size) =
                    *(U64*)(machine->buffer + address);

                machine->size += 8;
                counter += 2;
                break;
            }
            case INS_LOAD_N:
            {
                size_t address = (program + counter + 1)->imm_i64;
                size_t size = (program + counter + 2)->imm_i64;

                Machine_alloc(machine, size);

                memcpy(machine->buffer + machine->size,
                       machine->buffer + address, size);

                machine->size += size;
                counter += 3;
                break;
            }

            case INS_STORE_8:
            {
                size_t address = (program + counter + 1)->imm_i64;

                *(U8*)(machine->buffer + address) =
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 2;
                break;
            }
            case INS_STORE_16:
            {
                size_t address = (program + counter + 1)->imm_i64;

                *(U16*)(machine->buffer + address) =
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 2;
                break;
            }
            case INS_STORE_32:
            {
                size_t address = (program + counter + 1)->imm_i64;

                *(U32*)(machine->buffer + address) =
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 2;
                break;
            }
            case INS_STORE_64:
            {
                size_t address = (program + counter + 1)->imm_i64;

                *(U64*)(machine->buffer + address) =
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 2;
                break;
            }
            case INS_STORE_N:
            {
                size_t address = (program + counter + 1)->imm_i64;
                size_t size = (program + counter + 2)->imm_i64;

                memcpy(machine->buffer + address,
                       machine->buffer + machine->size - size, size);

                machine->size -= size;
                counter += 3;
                break;
            }

            case INS_MOV_8:
            {
                U8* ptr = (U8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 1, 1);

                machine->size -= 1;
                counter += 2;
                break;
            }
            case INS_MOV_16:
            {
                U8* ptr = (U8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 2, 2);

                machine->size -= 2;
                counter += 2;
                break;
            }
            case INS_MOV_32:
            {
                U8* ptr = (U8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 4, 4);

                machine->size -= 4;
                counter += 2;
                break;
            }
            case INS_MOV_64:
            {
                U8* ptr = (U8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 8, 8);

                machine->size -= 8;
                counter += 2;
                break;
            }
            case INS_MOV_N:
            {
                U8* ptr = (U8*)((program + counter + 1)->imm_i64);
                size_t size = (program + counter + 2)->imm_i64;

                memcpy(ptr, machine->buffer + machine->size - size, size);

                machine->size -= size;
                counter += 3;
                break;
            }

            case INS_INC_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) += 1;

                counter += 1;
                break;
            }
            case INS_INC_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) += 1;

                counter += 1;
                break;
            }
            case INS_INC_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) += 1;

                counter += 1;
                break;
            }
            case INS_INC_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) += 1;

                counter += 1;
                break;
            }

            case INS_DEC_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) -= 1;

                counter += 1;
                break;
            }
            case INS_DEC_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) -= 1;

                counter += 1;
                break;
            }
            case INS_DEC_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) -= 1;

                counter += 1;
                break;
            }
            case INS_DEC_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) -= 1;

                counter += 1;
                break;
            }

            case INS_OR_I8:
            {
                *(U8*)(machine->buffer + machine->size - 2) |=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_OR_I16:
            {
                *(U16*)(machine->buffer + machine->size - 4) |=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_OR_I32:
            {
                *(U32*)(machine->buffer + machine->size - 8) |=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_OR_I64:
            {
                *(U64*)(machine->buffer + machine->size - 16) |=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_ORI_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) |=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_ORI_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) |=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_ORI_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) |=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_ORI_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) |=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_XOR_I8:
            {
                *(U8*)(machine->buffer + machine->size - 2) ^=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_XOR_I16:
            {
                *(U16*)(machine->buffer + machine->size - 4) ^=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_XOR_I32:
            {
                *(U32*)(machine->buffer + machine->size - 8) ^=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_XOR_I64:
            {
                *(U64*)(machine->buffer + machine->size - 16) ^=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_XORI_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) ^=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_XORI_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) ^=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_XORI_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) ^=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_XORI_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) ^=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_AND_I8:
            {
                *(U8*)(machine->buffer + machine->size - 2) &=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_AND_I16:
            {
                *(U16*)(machine->buffer + machine->size - 4) &=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_AND_I32:
            {
                *(U32*)(machine->buffer + machine->size - 8) &=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_AND_I64:
            {
                *(U64*)(machine->buffer + machine->size - 16) &=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_ANDI_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) &=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) &=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) &=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) &=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_SHR_I8:
            {
                *(U8*)(machine->buffer + machine->size - 2) >>=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_SHR_I16:
            {
                *(U16*)(machine->buffer + machine->size - 4) >>=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_SHR_I32:
            {
                *(U32*)(machine->buffer + machine->size - 8) >>=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SHR_I64:
            {
                *(U64*)(machine->buffer + machine->size - 16) >>=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_SHRI_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) >>=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) >>=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) >>=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) >>=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_SHL_I8:
            {
                *(U8*)(machine->buffer + machine->size - 2) <<=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_SHL_I16:
            {
                *(U16*)(machine->buffer + machine->size - 4) <<=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_SHL_I32:
            {
                *(U32*)(machine->buffer + machine->size - 8) <<=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SHL_I64:
            {
                *(U64*)(machine->buffer + machine->size - 16) <<=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_SHLI_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) <<=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) <<=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) <<=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) <<=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_ADD_I8:
            {
                *(U8*)(machine->buffer + machine->size - 2) +=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_ADD_I16:
            {
                *(U16*)(machine->buffer + machine->size - 4) +=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_ADD_I32:
            {
                *(U32*)(machine->buffer + machine->size - 8) +=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_ADD_I64:
            {
                *(U64*)(machine->buffer + machine->size - 16) +=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_ADD_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 8) +=
                    *(FP32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_ADD_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 16) +=
                    *(FP64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_ADDI_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) +=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) +=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) +=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) +=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_ADDI_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 4) +=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_ADDI_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 8) +=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_SUB_I8:
            {
                *(U8*)(machine->buffer + machine->size - 2) -=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_SUB_I16:
            {
                *(U16*)(machine->buffer + machine->size - 4) -=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_SUB_I32:
            {
                *(U32*)(machine->buffer + machine->size - 8) -=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SUB_I64:
            {
                *(U64*)(machine->buffer + machine->size - 16) -=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_SUB_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 8) -=
                    *(FP32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SUB_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 16) -=
                    *(FP64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_SUBI_I8:
            {
                *(U8*)(machine->buffer + machine->size - 1) -=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I16:
            {
                *(U16*)(machine->buffer + machine->size - 2) -=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I32:
            {
                *(U32*)(machine->buffer + machine->size - 4) -=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I64:
            {
                *(U64*)(machine->buffer + machine->size - 8) -=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_SUBI_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 4) -=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_SUBI_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 8) -=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_MUL_U8:
            {
                *(U8*)(machine->buffer + machine->size - 2) *=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MUL_U16:
            {
                *(U16*)(machine->buffer + machine->size - 4) *=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MUL_U32:
            {
                *(U32*)(machine->buffer + machine->size - 8) *=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MUL_U64:
            {
                *(U64*)(machine->buffer + machine->size - 16) *=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_MUL_S8:
            {
                *(S8*)(machine->buffer + machine->size - 2) *=
                    *(S8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MUL_S16:
            {
                *(S16*)(machine->buffer + machine->size - 4) *=
                    *(S16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MUL_S32:
            {
                *(S32*)(machine->buffer + machine->size - 8) *=
                    *(S32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MUL_S64:
            {
                *(S64*)(machine->buffer + machine->size - 16) *=
                    *(S64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_MUL_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 8) *=
                    *(FP32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MUL_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 16) *=
                    *(FP64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_MULI_U8:
            {
                *(U8*)(machine->buffer + machine->size - 1) *=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_MULI_U16:
            {
                *(U16*)(machine->buffer + machine->size - 2) *=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_MULI_U32:
            {
                *(U32*)(machine->buffer + machine->size - 4) *=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_MULI_U64:
            {
                *(U64*)(machine->buffer + machine->size - 8) *=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_MULI_S8:
            {
                *(S8*)(machine->buffer + machine->size - 1) *=
                    (S8)((program + counter + 1)->imm_i8[0]);

                counter += 2;
                break;
            }
            case INS_MULI_S16:
            {
                *(S16*)(machine->buffer + machine->size - 2) *=
                    (S16)((program + counter + 1)->imm_i16[0]);

                counter += 2;
                break;
            }
            case INS_MULI_S32:
            {
                *(S32*)(machine->buffer + machine->size - 4) *=
                    (S32)((program + counter + 1)->imm_i32[0]);

                counter += 2;
                break;
            }
            case INS_MULI_S64:
            {
                *(S64*)(machine->buffer + machine->size - 8) *=
                    (S64)((program + counter + 1)->imm_i64);

                counter += 2;
                break;
            }
            case INS_MULI_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 4) *=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_MULI_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 8) *=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_DIV_U8:
            {
                *(U8*)(machine->buffer + machine->size - 2) /=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_DIV_U16:
            {
                *(U16*)(machine->buffer + machine->size - 4) /=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_DIV_U32:
            {
                *(U32*)(machine->buffer + machine->size - 8) /=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_DIV_U64:
            {
                *(U64*)(machine->buffer + machine->size - 16) /=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_DIV_S8:
            {
                *(S8*)(machine->buffer + machine->size - 2) /=
                    *(S8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_DIV_S16:
            {
                *(S16*)(machine->buffer + machine->size - 4) /=
                    *(S16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_DIV_S32:
            {
                *(S32*)(machine->buffer + machine->size - 8) /=
                    *(S32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_DIV_S64:
            {
                *(S64*)(machine->buffer + machine->size - 16) /=
                    *(S64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_DIV_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 8) /=
                    *(FP32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_DIV_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 16) /=
                    *(FP64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_DIVI_U8:
            {
                *(U8*)(machine->buffer + machine->size - 1) /=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U16:
            {
                *(U16*)(machine->buffer + machine->size - 2) /=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U32:
            {
                *(U32*)(machine->buffer + machine->size - 4) /=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U64:
            {
                *(U64*)(machine->buffer + machine->size - 8) /=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_DIVI_S8:
            {
                *(S8*)(machine->buffer + machine->size - 1) /=
                    (S8)((program + counter + 1)->imm_i8[0]);

                counter += 2;
                break;
            }
            case INS_DIVI_S16:
            {
                *(S16*)(machine->buffer + machine->size - 2) /=
                    (S16)((program + counter + 1)->imm_i16[0]);

                counter += 2;
                break;
            }
            case INS_DIVI_S32:
            {
                *(S32*)(machine->buffer + machine->size - 4) /=
                    (S32)((program + counter + 1)->imm_i32[0]);

                counter += 2;
                break;
            }
            case INS_DIVI_S64:
            {
                *(S64*)(machine->buffer + machine->size - 8) /=
                    (S64)((program + counter + 1)->imm_i64);

                counter += 2;
                break;
            }
            case INS_DIVI_FP32:
            {
                *(FP32*)(machine->buffer + machine->size - 4) /=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_DIVI_FP64:
            {
                *(FP64*)(machine->buffer + machine->size - 8) /=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_MOD_U8:
            {
                *(U8*)(machine->buffer + machine->size - 2) %=
                    *(U8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MOD_U16:
            {
                *(U16*)(machine->buffer + machine->size - 4) %=
                    *(U16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MOD_U32:
            {
                *(U32*)(machine->buffer + machine->size - 8) %=
                    *(U32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MOD_U64:
            {
                *(U64*)(machine->buffer + machine->size - 16) %=
                    *(U64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_MOD_S8:
            {
                *(S8*)(machine->buffer + machine->size - 2) %=
                    *(S8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MOD_S16:
            {
                *(S16*)(machine->buffer + machine->size - 4) %=
                    *(S16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MOD_S32:
            {
                *(S32*)(machine->buffer + machine->size - 8) %=
                    *(S32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MOD_S64:
            {
                *(S64*)(machine->buffer + machine->size - 16) %=
                    *(S64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_MODI_U8:
            {
                *(U8*)(machine->buffer + machine->size - 1) %=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_MODI_U16:
            {
                *(U16*)(machine->buffer + machine->size - 2) %=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_MODI_U32:
            {
                *(U32*)(machine->buffer + machine->size - 4) %=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_MODI_U64:
            {
                *(U64*)(machine->buffer + machine->size - 8) %=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_MODI_S8:
            {
                *(S8*)(machine->buffer + machine->size - 1) %=
                    (S8)((program + counter + 1)->imm_i8[0]);

                counter += 2;
                break;
            }
            case INS_MODI_S16:
            {
                *(S16*)(machine->buffer + machine->size - 2) %=
                    (S16)((program + counter + 1)->imm_i16[0]);

                counter += 2;
                break;
            }
            case INS_MODI_S32:
            {
                *(S32*)(machine->buffer + machine->size - 4) %=
                    (S32)((program + counter + 1)->imm_i32[0]);

                counter += 2;
                break;
            }
            case INS_MODI_S64:
            {
                *(S64*)(machine->buffer + machine->size - 8) %=
                    (S64)((program + counter + 1)->imm_i64);

                counter += 2;
                break;
            }

            case INS_CMP_U8:
            {
                U8 a = *(U8*)(machine->buffer + machine->size - 2);
                U8 b = *(U8*)(machine->buffer + machine->size - 1);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U16:
            {
                U16 a = *(U16*)(machine->buffer + machine->size - 4);
                U16 b = *(U16*)(machine->buffer + machine->size - 2);

                *(U8*)(machine->buffer + machine->size - 4) =
                    (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U32:
            {
                U32 a = *(U32*)(machine->buffer + machine->size - 8);
                U32 b = *(U32*)(machine->buffer + machine->size - 4);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U64:
            {
                U64 a = *(U64*)(machine->buffer + machine->size - 16);
                U64 b = *(U64*)(machine->buffer + machine->size - 8);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S8:
            {
                S8 a = *(S8*)(machine->buffer + machine->size - 2);
                S8 b = *(S8*)(machine->buffer + machine->size - 1);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S16:
            {
                S16 a = *(S16*)(machine->buffer + machine->size - 4);
                S16 b = *(S16*)(machine->buffer + machine->size - 2);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S32:
            {
                S32 a = *(S32*)(machine->buffer + machine->size - 8);
                S32 b = *(S32*)(machine->buffer + machine->size - 4);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S64:
            {
                S64 a = *(S64*)(machine->buffer + machine->size - 16);
                S64 b = *(S64*)(machine->buffer + machine->size - 8);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_FP32:
            {
                FP32 a = *(FP32*)(machine->buffer + machine->size - 8);
                FP32 b = *(FP32*)(machine->buffer + machine->size - 4);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_FP64:
            {
                FP64 a = *(FP64*)(machine->buffer + machine->size - 16);
                FP64 b = *(FP64*)(machine->buffer + machine->size - 8);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMPI_U8:
            {
                U8 a = *(U8*)(machine->buffer + machine->size - 1);
                U8 b = (program + counter + 1)->imm_i8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U16:
            {
                U16 a = *(U16*)(machine->buffer + machine->size - 2);
                U16 b = (program + counter + 1)->imm_i16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U32:
            {
                U32 a = *(U32*)(machine->buffer + machine->size - 4);
                U32 b = (program + counter + 1)->imm_i32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U64:
            {
                U64 a = *(U64*)(machine->buffer + machine->size - 8);
                U64 b = (program + counter + 1)->imm_i64;

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S8:
            {
                S8 a = *(S8*)(machine->buffer + machine->size - 1);
                S8 b = (S8)((program + counter + 1)->imm_i8[0]);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S16:
            {
                S16 a = *(S16*)(machine->buffer + machine->size - 2);
                S16 b = (S16)((program + counter + 1)->imm_i16[0]);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S32:
            {
                S32 a = *(S32*)(machine->buffer + machine->size - 4);
                S32 b = (S32)((program + counter + 1)->imm_i32[0]);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S64:
            {
                S64 a = *(S64*)(machine->buffer + machine->size - 8);
                S64 b = (S64)((program + counter + 1)->imm_i64);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP32:
            {
                FP32 a = *(FP32*)(machine->buffer + machine->size - 4);
                FP32 b = (program + counter + 1)->imm_fp32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP64:
            {
                FP64 a = *(FP64*)(machine->buffer + machine->size - 8);
                FP64 b = (program + counter + 1)->imm_fp64;

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_JEQ:
            {
                if (machine->cmp == 0)
                {
                    counter = (program + counter + 1)->imm_i64;
                }
                else
                {
                    counter += 2;
                }

                break;
            }
            case INS_JNE:
            {
                if (machine->cmp != 0)
                {
                    counter = (program + counter + 1)->imm_i64;
                }
                else
                {
                    counter += 2;
                }

                break;
            }
            case INS_JLT:
            {
                if (machine->cmp < 0)
                {
                    counter = (program + counter + 1)->imm_i64;
                }
                else
                {
                    counter += 2;
                }

                break;
            }
            case INS_JLE:
            {
                if (machine->cmp <= 0)
                {
                    counter = (program + counter + 1)->imm_i64;
                }
                else
                {
                    counter += 2;
                }

                break;
            }
            case INS_JGT:
            {
                if (machine->cmp > 0)
                {
                    counter = (program + counter + 1)->imm_i64;
                }
                else
                {
                    counter += 2;
                }

                break;
            }
            case INS_JGE:
            {
                if (machine->cmp >= 0)
                {
                    counter = (program + counter + 1)->imm_i64;
                }
                else
                {
                    counter += 2;
                }

                break;
            }
            case INS_JMP:
            {
                counter = (program + counter + 1)->imm_i64;
                break;
            }
            case INS_NATIVE_CALL:
            {
                U64 number_of_parameters = (program + counter + 3)->imm_i64;

                if (machine->size + ((number_of_parameters + 1) * 2) >
                    machine->capacity)
                {
                    U8* buffer =
                        (U8*)malloc(sizeof(U8) * ((machine->size + 1) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                void* function_pointer =
                    (void*)((program + counter + 1)->imm_i64);

                U64 return_type = (program + counter + 2)->imm_i32[0];
                U64 return_size = (program + counter + 2)->imm_i32[1];

                U8* ptr = machine->buffer + machine->size;

                size_t amount_to_shrink = 0;

                for (size_t it = number_of_parameters; it > 0; it--)
                {
                    U64 param_type = (program + counter + it + 3)->imm_i32[0];
                    U64 param_size = (program + counter + it + 3)->imm_i32[1];

                    Call_Type_And_Value* param =
                        (Call_Type_And_Value*)(machine->buffer + machine->size +
                                               (it *
                                                sizeof(Call_Type_And_Value)));

                    ptr -= param_size;

                    switch (param_type)
                    {
                        case Program_Type_I8:
                        {
                            param->type = Call_Type_I8;
                            param->value.u8[0] = *(U8*)(ptr);
                            break;
                        }
                        case Program_Type_I16:
                        {
                            param->type = Call_Type_I16;
                            param->value.u16[0] = *(U16*)(ptr);
                            break;
                        }
                        case Program_Type_I32:
                        {
                            param->type = Call_Type_I32;
                            param->value.u32[0] = *(U32*)(ptr);
                            break;
                        }
                        case Program_Type_I64:
                        {
                            param->type = Call_Type_I64;
                            param->value.u64 = *(U64*)(ptr);
                            break;
                        }
                        case Program_Type_FP32:
                        {
                            param->type = Call_Type_FP32;
                            param->value.fp32[0] = *(FP32*)(ptr);
                            break;
                        }
                        case Program_Type_FP64:
                        {
                            param->type = Call_Type_FP64;
                            param->value.fp64 = *(FP64*)(ptr);
                            break;
                        }
                        case Program_Type_Struct:
                        {
                            switch (param_size)
                            {
                                case 1:
                                {
                                    param->type = Call_Type_I8;
                                    param->value.u8[0] = *(U8*)(ptr);
                                    break;
                                }
                                case 2:
                                {
                                    param->type = Call_Type_I16;
                                    param->value.u16[0] = *(U16*)(ptr);
                                    break;
                                }
                                case 4:
                                {
                                    param->type = Call_Type_I32;
                                    param->value.u32[0] = *(U32*)(ptr);
                                    break;
                                }
                                case 8:
                                {
                                    param->type = Call_Type_I64;
                                    param->value.u64 = *(U64*)(ptr);
                                    break;
                                }
                                default:
                                {
                                    param->type = Call_Type_Struct;
                                    param->value.ptr = ptr;
                                    break;
                                }
                            }

                            break;
                        }
                    }

                    auto a = 1;
                }

                Call_Type_And_Value* ret =
                    (Call_Type_And_Value*)(machine->buffer + machine->size);

                switch (return_type)
                {
                    case Program_Type_I8:
                    {
                        ret->type = Call_Type_I8;
                        break;
                    }
                    case Program_Type_I16:
                    {
                        ret->type = Call_Type_I16;
                        break;
                    }
                    case Program_Type_I32:
                    {
                        ret->type = Call_Type_I32;
                        break;
                    }
                    case Program_Type_I64:
                    {
                        ret->type = Call_Type_I64;
                        break;
                    }
                    case Program_Type_FP32:
                    {
                        ret->type = Call_Type_FP32;
                        break;
                    }
                    case Program_Type_FP64:
                    {
                        ret->type = Call_Type_FP64;
                        break;
                    }
                    case Program_Type_Struct:
                    {
                        switch (return_size)
                        {
                            case 1:
                            {
                                ret->type = Call_Type_I8;
                                break;
                            }
                            case 2:
                            {
                                ret->type = Call_Type_I16;
                                break;
                            }
                            case 4:
                            {
                                ret->type = Call_Type_I32;
                                break;
                            }
                            case 8:
                            {
                                ret->type = Call_Type_I64;
                                break;
                            }
                            default:
                            {
                                ret->type = Call_Type_Struct;
                                break;
                            }
                        }

                        break;
                    }
                }

                ret->type = return_type;
                ret->value.ptr = ptr;

                Native_Call(function_pointer, *ret, number_of_parameters,
                            ret + 1);

                machine->size -=
                    (size_t)((machine->buffer + machine->size) - (ptr));
                counter += 4 + number_of_parameters;
                break;
            }
            default:
            {
                assert(False && "Invalid Instruction");
            }
        }
    }
}
