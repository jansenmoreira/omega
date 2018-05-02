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
                if (machine->size + 1 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 1) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                machine->size += 1;
                counter += 1;
                break;
            }
            case INS_ALLOC_16:
            {
                if (machine->size + 2 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 2) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                machine->size += 2;
                counter += 1;
                break;
            }
            case INS_ALLOC_32:
            {
                if (machine->size + 4 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 4) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                machine->size += 4;
                counter += 1;
                break;
            }
            case INS_ALLOC_64:
            {
                if (machine->size + 8 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 8) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                machine->size += 8;
                counter += 1;
                break;
            }
            case INS_ALLOC_N:
            {
                u64 size = (program + counter + 1)->imm_i64;

                if (machine->size + size > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + size) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                machine->size += size;
                counter += 2;
                break;
            }

            case INS_PUSH_8:
            {
                if (machine->size + 1 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 1) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                u8 value = (program + counter + 1)->imm_i8[0];
                *(u8*)(machine->buffer + machine->size) = value;

                machine->size += 1;
                counter += 2;
                break;
            }
            case INS_PUSH_16:
            {
                if (machine->size + 2 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 2) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                u16 value = (program + counter + 1)->imm_i16[0];
                *(u16*)(machine->buffer + machine->size) = value;

                machine->size += 2;
                counter += 2;
                break;
            }
            case INS_PUSH_32:
            {
                if (machine->size + 4 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 4) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                u32 value = (program + counter + 1)->imm_i32[0];
                *(u32*)(machine->buffer + machine->size) = value;

                machine->size += 4;
                counter += 2;
                break;
            }
            case INS_PUSH_64:
            {
                if (machine->size + 8 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 8) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                u64 value = (program + counter + 1)->imm_i64;
                *(u64*)(machine->buffer + machine->size) = value;

                machine->size += 8;
                counter += 2;
                break;
            }
            case INS_PUSH_N:
            {
                u8* ptr = (u8*)((program + counter + 1)->imm_i64);
                u64 size = (program + counter + 2)->imm_i64;

                if (machine->size + 8 > machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 8) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                memcpy(machine->buffer + machine->size, ptr, size);

                machine->size += size;
                counter += 3;
                break;
            }
            case INS_POP_8:
            {
                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_POP_16:
            {
                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_POP_32:
            {
                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_POP_64:
            {
                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_POP_N:
            {
                machine->size -= (program + counter + 1)->imm_i64;
                counter += 2;
                break;
            }
            case INS_MOV_8:
            {
                u8* ptr = (u8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 1, 1);

                machine->size -= 1;
                counter += 2;
                break;
            }
            case INS_MOV_16:
            {
                u8* ptr = (u8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 2, 2);

                machine->size -= 2;
                counter += 2;
                break;
            }
            case INS_MOV_32:
            {
                u8* ptr = (u8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 4, 4);

                machine->size -= 4;
                counter += 2;
                break;
            }
            case INS_MOV_64:
            {
                u8* ptr = (u8*)((program + counter + 1)->imm_i64);

                memcpy(ptr, machine->buffer + machine->size - 8, 8);

                machine->size -= 8;
                counter += 2;
                break;
            }
            case INS_MOV_N:
            {
                u8* ptr = (u8*)((program + counter + 1)->imm_i64);
                size_t size = (program + counter + 2)->imm_i64;

                memcpy(ptr, machine->buffer + machine->size - size, size);

                machine->size -= size;
                counter += 3;
                break;
            }

            case INS_INC_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) += 1;

                counter += 1;
                break;
            }
            case INS_INC_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) += 1;

                counter += 1;
                break;
            }
            case INS_INC_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) += 1;

                counter += 1;
                break;
            }
            case INS_INC_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) += 1;

                counter += 1;
                break;
            }

            case INS_DEC_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) -= 1;

                counter += 1;
                break;
            }
            case INS_DEC_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) -= 1;

                counter += 1;
                break;
            }
            case INS_DEC_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) -= 1;

                counter += 1;
                break;
            }
            case INS_DEC_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) -= 1;

                counter += 1;
                break;
            }

            case INS_OR_I8:
            {
                *(u8*)(machine->buffer + machine->size - 2) |=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_OR_I16:
            {
                *(u16*)(machine->buffer + machine->size - 4) |=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_OR_I32:
            {
                *(u32*)(machine->buffer + machine->size - 8) |=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_OR_I64:
            {
                *(u64*)(machine->buffer + machine->size - 16) |=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_ORI_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) |=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_ORI_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) |=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_ORI_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) |=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_ORI_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) |=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_XOR_I8:
            {
                *(u8*)(machine->buffer + machine->size - 2) ^=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_XOR_I16:
            {
                *(u16*)(machine->buffer + machine->size - 4) ^=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_XOR_I32:
            {
                *(u32*)(machine->buffer + machine->size - 8) ^=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_XOR_I64:
            {
                *(u64*)(machine->buffer + machine->size - 16) ^=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_XORI_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) ^=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_XORI_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) ^=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_XORI_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) ^=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_XORI_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) ^=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_AND_I8:
            {
                *(u8*)(machine->buffer + machine->size - 2) &=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_AND_I16:
            {
                *(u16*)(machine->buffer + machine->size - 4) &=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_AND_I32:
            {
                *(u32*)(machine->buffer + machine->size - 8) &=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_AND_I64:
            {
                *(u64*)(machine->buffer + machine->size - 16) &=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_ANDI_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) &=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) &=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) &=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) &=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_SHR_I8:
            {
                *(u8*)(machine->buffer + machine->size - 2) >>=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_SHR_I16:
            {
                *(u16*)(machine->buffer + machine->size - 4) >>=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_SHR_I32:
            {
                *(u32*)(machine->buffer + machine->size - 8) >>=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SHR_I64:
            {
                *(u64*)(machine->buffer + machine->size - 16) >>=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_SHRI_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) >>=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) >>=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) >>=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) >>=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_SHL_I8:
            {
                *(u8*)(machine->buffer + machine->size - 2) <<=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_SHL_I16:
            {
                *(u16*)(machine->buffer + machine->size - 4) <<=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_SHL_I32:
            {
                *(u32*)(machine->buffer + machine->size - 8) <<=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SHL_I64:
            {
                *(u64*)(machine->buffer + machine->size - 16) <<=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_SHLI_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) <<=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) <<=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) <<=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) <<=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }

            case INS_ADD_I8:
            {
                *(u8*)(machine->buffer + machine->size - 2) +=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_ADD_I16:
            {
                *(u16*)(machine->buffer + machine->size - 4) +=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_ADD_I32:
            {
                *(u32*)(machine->buffer + machine->size - 8) +=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_ADD_I64:
            {
                *(u64*)(machine->buffer + machine->size - 16) +=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_ADD_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 8) +=
                    *(fp32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_ADD_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 16) +=
                    *(fp64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_ADDI_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) +=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) +=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) +=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) +=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_ADDI_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 4) +=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_ADDI_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 8) +=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_SUB_I8:
            {
                *(u8*)(machine->buffer + machine->size - 2) -=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_SUB_I16:
            {
                *(u16*)(machine->buffer + machine->size - 4) -=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_SUB_I32:
            {
                *(u32*)(machine->buffer + machine->size - 8) -=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SUB_I64:
            {
                *(u64*)(machine->buffer + machine->size - 16) -=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_SUB_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 8) -=
                    *(fp32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_SUB_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 16) -=
                    *(fp64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_SUBI_I8:
            {
                *(u8*)(machine->buffer + machine->size - 1) -=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I16:
            {
                *(u16*)(machine->buffer + machine->size - 2) -=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I32:
            {
                *(u32*)(machine->buffer + machine->size - 4) -=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I64:
            {
                *(u64*)(machine->buffer + machine->size - 8) -=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_SUBI_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 4) -=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_SUBI_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 8) -=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_MUL_U8:
            {
                *(u8*)(machine->buffer + machine->size - 2) *=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MUL_U16:
            {
                *(u16*)(machine->buffer + machine->size - 4) *=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MUL_U32:
            {
                *(u32*)(machine->buffer + machine->size - 8) *=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MUL_U64:
            {
                *(u64*)(machine->buffer + machine->size - 16) *=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_MUL_S8:
            {
                *(s8*)(machine->buffer + machine->size - 2) *=
                    *(s8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MUL_S16:
            {
                *(s16*)(machine->buffer + machine->size - 4) *=
                    *(s16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MUL_S32:
            {
                *(s32*)(machine->buffer + machine->size - 8) *=
                    *(s32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MUL_S64:
            {
                *(s64*)(machine->buffer + machine->size - 16) *=
                    *(s64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_MUL_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 8) *=
                    *(fp32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MUL_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 16) *=
                    *(fp64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_MULI_U8:
            {
                *(u8*)(machine->buffer + machine->size - 1) *=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_MULI_U16:
            {
                *(u16*)(machine->buffer + machine->size - 2) *=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_MULI_U32:
            {
                *(u32*)(machine->buffer + machine->size - 4) *=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_MULI_U64:
            {
                *(u64*)(machine->buffer + machine->size - 8) *=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_MULI_S8:
            {
                *(s8*)(machine->buffer + machine->size - 1) *=
                    (s8)((program + counter + 1)->imm_i8[0]);

                counter += 2;
                break;
            }
            case INS_MULI_S16:
            {
                *(s16*)(machine->buffer + machine->size - 2) *=
                    (s16)((program + counter + 1)->imm_i16[0]);

                counter += 2;
                break;
            }
            case INS_MULI_S32:
            {
                *(s32*)(machine->buffer + machine->size - 4) *=
                    (s32)((program + counter + 1)->imm_i32[0]);

                counter += 2;
                break;
            }
            case INS_MULI_S64:
            {
                *(s64*)(machine->buffer + machine->size - 8) *=
                    (s64)((program + counter + 1)->imm_i64);

                counter += 2;
                break;
            }
            case INS_MULI_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 4) *=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_MULI_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 8) *=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_DIV_U8:
            {
                *(u8*)(machine->buffer + machine->size - 2) /=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_DIV_U16:
            {
                *(u16*)(machine->buffer + machine->size - 4) /=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_DIV_U32:
            {
                *(u32*)(machine->buffer + machine->size - 8) /=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_DIV_U64:
            {
                *(u64*)(machine->buffer + machine->size - 16) /=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_DIV_S8:
            {
                *(s8*)(machine->buffer + machine->size - 2) /=
                    *(s8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_DIV_S16:
            {
                *(s16*)(machine->buffer + machine->size - 4) /=
                    *(s16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_DIV_S32:
            {
                *(s32*)(machine->buffer + machine->size - 8) /=
                    *(s32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_DIV_S64:
            {
                *(s64*)(machine->buffer + machine->size - 16) /=
                    *(s64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_DIV_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 8) /=
                    *(fp32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_DIV_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 16) /=
                    *(fp64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_DIVI_U8:
            {
                *(u8*)(machine->buffer + machine->size - 1) /=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U16:
            {
                *(u16*)(machine->buffer + machine->size - 2) /=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U32:
            {
                *(u32*)(machine->buffer + machine->size - 4) /=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U64:
            {
                *(u64*)(machine->buffer + machine->size - 8) /=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_DIVI_S8:
            {
                *(s8*)(machine->buffer + machine->size - 1) /=
                    (s8)((program + counter + 1)->imm_i8[0]);

                counter += 2;
                break;
            }
            case INS_DIVI_S16:
            {
                *(s16*)(machine->buffer + machine->size - 2) /=
                    (s16)((program + counter + 1)->imm_i16[0]);

                counter += 2;
                break;
            }
            case INS_DIVI_S32:
            {
                *(s32*)(machine->buffer + machine->size - 4) /=
                    (s32)((program + counter + 1)->imm_i32[0]);

                counter += 2;
                break;
            }
            case INS_DIVI_S64:
            {
                *(s64*)(machine->buffer + machine->size - 8) /=
                    (s64)((program + counter + 1)->imm_i64);

                counter += 2;
                break;
            }
            case INS_DIVI_FP32:
            {
                *(fp32*)(machine->buffer + machine->size - 4) /=
                    (program + counter + 1)->imm_fp32[0];

                counter += 2;
                break;
            }
            case INS_DIVI_FP64:
            {
                *(fp64*)(machine->buffer + machine->size - 8) /=
                    (program + counter + 1)->imm_fp64;

                counter += 2;
                break;
            }

            case INS_MOD_U8:
            {
                *(u8*)(machine->buffer + machine->size - 2) %=
                    *(u8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MOD_U16:
            {
                *(u16*)(machine->buffer + machine->size - 4) %=
                    *(u16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MOD_U32:
            {
                *(u32*)(machine->buffer + machine->size - 8) %=
                    *(u32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MOD_U64:
            {
                *(u64*)(machine->buffer + machine->size - 16) %=
                    *(u64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }
            case INS_MOD_S8:
            {
                *(s8*)(machine->buffer + machine->size - 2) %=
                    *(s8*)(machine->buffer + machine->size - 1);

                machine->size -= 1;
                counter += 1;
                break;
            }
            case INS_MOD_S16:
            {
                *(s16*)(machine->buffer + machine->size - 4) %=
                    *(s16*)(machine->buffer + machine->size - 2);

                machine->size -= 2;
                counter += 1;
                break;
            }
            case INS_MOD_S32:
            {
                *(s32*)(machine->buffer + machine->size - 8) %=
                    *(s32*)(machine->buffer + machine->size - 4);

                machine->size -= 4;
                counter += 1;
                break;
            }
            case INS_MOD_S64:
            {
                *(s64*)(machine->buffer + machine->size - 16) %=
                    *(s64*)(machine->buffer + machine->size - 8);

                machine->size -= 8;
                counter += 1;
                break;
            }

            case INS_MODI_U8:
            {
                *(u8*)(machine->buffer + machine->size - 1) %=
                    (program + counter + 1)->imm_i8[0];

                counter += 2;
                break;
            }
            case INS_MODI_U16:
            {
                *(u16*)(machine->buffer + machine->size - 2) %=
                    (program + counter + 1)->imm_i16[0];

                counter += 2;
                break;
            }
            case INS_MODI_U32:
            {
                *(u32*)(machine->buffer + machine->size - 4) %=
                    (program + counter + 1)->imm_i32[0];

                counter += 2;
                break;
            }
            case INS_MODI_U64:
            {
                *(u64*)(machine->buffer + machine->size - 8) %=
                    (program + counter + 1)->imm_i64;

                counter += 2;
                break;
            }
            case INS_MODI_S8:
            {
                *(s8*)(machine->buffer + machine->size - 1) %=
                    (s8)((program + counter + 1)->imm_i8[0]);

                counter += 2;
                break;
            }
            case INS_MODI_S16:
            {
                *(s16*)(machine->buffer + machine->size - 2) %=
                    (s16)((program + counter + 1)->imm_i16[0]);

                counter += 2;
                break;
            }
            case INS_MODI_S32:
            {
                *(s32*)(machine->buffer + machine->size - 4) %=
                    (s32)((program + counter + 1)->imm_i32[0]);

                counter += 2;
                break;
            }
            case INS_MODI_S64:
            {
                *(s64*)(machine->buffer + machine->size - 8) %=
                    (s64)((program + counter + 1)->imm_i64);

                counter += 2;
                break;
            }

            case INS_CMP_U8:
            {
                u8 a = *(u8*)(machine->buffer + machine->size - 2);
                u8 b = *(u8*)(machine->buffer + machine->size - 1);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U16:
            {
                u16 a = *(u16*)(machine->buffer + machine->size - 4);
                u16 b = *(u16*)(machine->buffer + machine->size - 2);

                *(u8*)(machine->buffer + machine->size - 4) =
                    (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U32:
            {
                u32 a = *(u32*)(machine->buffer + machine->size - 8);
                u32 b = *(u32*)(machine->buffer + machine->size - 4);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U64:
            {
                u64 a = *(u64*)(machine->buffer + machine->size - 16);
                u64 b = *(u64*)(machine->buffer + machine->size - 8);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S8:
            {
                s8 a = *(s8*)(machine->buffer + machine->size - 2);
                s8 b = *(s8*)(machine->buffer + machine->size - 1);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S16:
            {
                s16 a = *(s16*)(machine->buffer + machine->size - 4);
                s16 b = *(s16*)(machine->buffer + machine->size - 2);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S32:
            {
                s32 a = *(s32*)(machine->buffer + machine->size - 8);
                s32 b = *(s32*)(machine->buffer + machine->size - 4);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S64:
            {
                s64 a = *(s64*)(machine->buffer + machine->size - 16);
                s64 b = *(s64*)(machine->buffer + machine->size - 8);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_FP32:
            {
                fp32 a = *(fp32*)(machine->buffer + machine->size - 8);
                fp32 b = *(fp32*)(machine->buffer + machine->size - 4);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_FP64:
            {
                fp64 a = *(fp64*)(machine->buffer + machine->size - 16);
                fp64 b = *(fp64*)(machine->buffer + machine->size - 8);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMPI_U8:
            {
                u8 a = *(u8*)(machine->buffer + machine->size - 1);
                u8 b = (program + counter + 1)->imm_i8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U16:
            {
                u16 a = *(u16*)(machine->buffer + machine->size - 2);
                u16 b = (program + counter + 1)->imm_i16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U32:
            {
                u32 a = *(u32*)(machine->buffer + machine->size - 4);
                u32 b = (program + counter + 1)->imm_i32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U64:
            {
                u64 a = *(u64*)(machine->buffer + machine->size - 8);
                u64 b = (program + counter + 1)->imm_i64;

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S8:
            {
                s8 a = *(s8*)(machine->buffer + machine->size - 1);
                s8 b = (s8)((program + counter + 1)->imm_i8[0]);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S16:
            {
                s16 a = *(s16*)(machine->buffer + machine->size - 2);
                s16 b = (s16)((program + counter + 1)->imm_i16[0]);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S32:
            {
                s32 a = *(s32*)(machine->buffer + machine->size - 4);
                s32 b = (s32)((program + counter + 1)->imm_i32[0]);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S64:
            {
                s64 a = *(s64*)(machine->buffer + machine->size - 8);
                s64 b = (s64)((program + counter + 1)->imm_i64);

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP32:
            {
                fp32 a = *(fp32*)(machine->buffer + machine->size - 4);
                fp32 b = (program + counter + 1)->imm_fp32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP64:
            {
                fp64 a = *(fp64*)(machine->buffer + machine->size - 8);
                fp64 b = (program + counter + 1)->imm_fp64;

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
                u64 number_of_parameters = (program + counter + 3)->imm_i64;

                if (machine->size + ((number_of_parameters + 1) * 2) >
                    machine->capacity)
                {
                    u8* buffer =
                        (u8*)malloc(sizeof(u8) * ((machine->size + 1) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->buffer, machine->size);

                    machine->buffer = buffer;
                }

                void* function_pointer =
                    (void*)((program + counter + 1)->imm_i64);

                u32 return_type = (program + counter + 2)->imm_i32[0];
                u32 return_size = (program + counter + 2)->imm_i32[1];

                u8* ptr = machine->buffer + machine->size;

                size_t adjust = 0;

                for (size_t it = number_of_parameters; it > 0; it--)
                {
                    u32 param_type = (program + counter + it + 3)->imm_i32[0];
                    u32 param_size = (program + counter + it + 3)->imm_i32[1];

                    ptr -= param_size;
                    adjust += param_size;

                    Native_Call_Value* param =
                        (Native_Call_Value*)(machine->buffer + machine->size +
                                             (it * sizeof(Native_Call_Value)));
                    param->type = param_type;
                    param->size = param_size;
                    param->value = ptr;
                }

                ptr -= return_size;

                Native_Call_Value* ret =
                    (Native_Call_Value*)(machine->buffer + machine->size);
                ret->type = return_type;
                ret->size = return_size;
                ret->value = ptr;

                Native_Call(function_pointer, *ret, number_of_parameters,
                            ret + 1);

                machine->size -= adjust;
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
