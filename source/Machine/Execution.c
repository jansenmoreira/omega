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

            case INS_ALLOC:
            {
                size_t size = program[counter + 1].immediate.u64[0];

                size = (size & 0x7) ? (size | 0x7) + 1 : size;

                Value* address = (Value*)malloc(size);
                Machine_grow_local(machine, 1);
                machine->local.buffer[machine->local.size++] = address;
                counter += 2;
                break;
            }

            case INS_FREE:
            {
                Value* address = machine->local.buffer[--machine->local.size];
                free(address);
                counter += 1;
                break;
            }

            case INS_LDG_8:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                PTR address = machine->global.buffer[index];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u8[0] =
                    *(U8*)(address);

                counter += 2;
                break;
            }

            case INS_LDG_16:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                PTR address = machine->global.buffer[index];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u16[0] =
                    *(U16*)(address);

                counter += 2;
                break;
            }

            case INS_LDG_32:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                PTR address = machine->global.buffer[index];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u32[0] =
                    *(U32*)(address);

                counter += 2;
                break;
            }

            case INS_LDG_64:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                PTR address = machine->global.buffer[index];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u64[0] =
                    *(U64*)(address);

                counter += 2;
                break;
            }

            case INS_STG_8:
            {
                size_t index = program[counter + 1].immediate.u64[0];

                *(U8*)(machine->global.buffer[index]) =
                    machine->stack.buffer[--machine->stack.size].u8[0];

                counter += 2;
                break;
            }

            case INS_STG_16:
            {
                size_t index = program[counter + 1].immediate.u64[0];

                *(U16*)(machine->global.buffer[index]) =
                    machine->stack.buffer[--machine->stack.size].u16[0];

                counter += 2;
                break;
            }

            case INS_STG_32:
            {
                size_t index = program[counter + 1].immediate.u64[0];

                *(U32*)(machine->global.buffer[index]) =
                    machine->stack.buffer[--machine->stack.size].u32[0];

                counter += 2;
                break;
            }

            case INS_STG_64:
            {
                size_t index = program[counter + 1].immediate.u64[0];

                *(U64*)(machine->global.buffer[index]) =
                    machine->stack.buffer[--machine->stack.size].u64[0];

                counter += 2;
                break;
            }

            case INS_LEAG:
            {
                size_t index = program[counter + 1].immediate.u64[0];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].ptr[0] =
                    machine->global.buffer[index];

                counter += 2;
                break;
            }

            case INS_LDL_8:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;
                size_t low = index & 0x0000000000000007ULL;

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u8[0] =
                    address[high].u8[low];

                counter += 2;
                break;
            }

            case INS_LDL_16:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;
                size_t low = (index & 0x0000000000000006ULL) >> 1ULL;

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u16[0] =
                    address[high].u16[low];

                counter += 2;
                break;
            }

            case INS_LDL_32:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;
                size_t low = (index & 0x0000000000000004ULL) >> 2ULL;

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u32[0] =
                    address[high].u32[low];

                counter += 2;
                break;
            }

            case INS_LDL_64:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u64[0] =
                    address[high].u64[0];

                counter += 2;
                break;
            }

            case INS_STL_8:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;
                size_t low = index & 0x0000000000000007ULL;

                address[high].u8[low] =
                    machine->stack.buffer[--machine->stack.size].u8[0];

                counter += 2;
                break;
            }

            case INS_STL_16:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;
                size_t low = (index & 0x0000000000000006ULL) >> 1ULL;

                address[high].u16[low] =
                    machine->stack.buffer[--machine->stack.size].u16[0];

                counter += 2;
                break;
            }

            case INS_STL_32:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;
                size_t low = (index & 0x0000000000000004ULL) >> 2ULL;

                address[high].u32[low] =
                    machine->stack.buffer[--machine->stack.size].u32[0];

                counter += 2;
                break;
            }

            case INS_STL_64:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;

                address[high].u64[0] =
                    machine->stack.buffer[--machine->stack.size].u64[0];

                counter += 2;
                break;
            }

            case INS_LEAL:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                Value* address = machine->local.buffer[machine->local.size - 1];

                size_t high = (index & 0xFFFFFFFFFFFFFFF8ULL) >> 3ULL;

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].ptr[0] =
                    (PTR)(&address[high]);

                counter += 2;
                break;
            }

            case INS_LDI_8:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u8[0] =
                    *(U8*)(address);

                counter += 2;
                break;
            }

            case INS_LDI_16:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u16[0] =
                    *(U16*)(address);

                counter += 2;
                break;
            }

            case INS_LDI_32:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u32[0] =
                    *(U32*)(address);

                counter += 2;
                break;
            }

            case INS_LDI_64:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u64[0] =
                    *(U64*)(address);

                counter += 2;
                break;
            }

            case INS_STI_8:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                *(U8*)(address) =
                    machine->stack.buffer[--machine->stack.size].u8[0];

                counter += 2;
                break;
            }

            case INS_STI_16:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                *(U16*)(address) =
                    machine->stack.buffer[--machine->stack.size].u16[0];

                counter += 2;
                break;
            }

            case INS_STI_32:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                *(U32*)(address) =
                    machine->stack.buffer[--machine->stack.size].u32[0];

                counter += 2;
                break;
            }

            case INS_STI_64:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                *(U64*)(address) =
                    machine->stack.buffer[--machine->stack.size].u64[0];

                counter += 2;
                break;
            }

            case INS_LD_8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] = *(
                    U8*)(machine->stack.buffer[machine->stack.size - 1].ptr[0]);

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_LD_16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] =
                    *(U16*)(machine->stack.buffer[machine->stack.size - 1]
                                .ptr[0]);

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_LD_32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] =
                    *(U32*)(machine->stack.buffer[machine->stack.size - 1]
                                .ptr[0]);

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_LD_64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] =
                    *(U64*)(machine->stack.buffer[machine->stack.size - 1]
                                .ptr[0]);

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_ST_8:
            {
                *(U8*)(machine->stack.buffer[machine->stack.size - 1].ptr[0]) =
                    machine->stack.buffer[machine->stack.size - 2].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ST_16:
            {
                *(U16*)(machine->stack.buffer[machine->stack.size - 1].ptr[0]) =
                    machine->stack.buffer[machine->stack.size - 2].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ST_32:
            {
                *(U32*)(machine->stack.buffer[machine->stack.size - 1].ptr[0]) =
                    machine->stack.buffer[machine->stack.size - 2].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ST_64:
            {
                *(U64*)(machine->stack.buffer[machine->stack.size - 1].ptr[0]) =
                    machine->stack.buffer[machine->stack.size - 2].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_COPY:
            {
                U64 size = program[counter + 1].immediate.u64[0];

                PTR source =
                    machine->stack.buffer[machine->stack.size - 1].ptr[0];
                PTR destination =
                    machine->stack.buffer[machine->stack.size - 2].ptr[0];

                memcpy(destination, source, size);

                machine->stack.size -= 2;
                counter += 2;
                break;
            }

            case INS_PUSH_8:
            {
                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u8[0] =
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_PUSH_16:
            {
                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u16[0] =
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_PUSH_32:
            {
                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u32[0] =
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_PUSH_64:
            {
                Machine_grow_stack(machine, 1);

                machine->stack.buffer[machine->stack.size++].u64[0] =
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }

            case INS_POP:
            {
                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_INC_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] += 1;
                counter += 1;
                break;
            }
            case INS_INC_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] += 1;
                counter += 1;
                break;
            }
            case INS_INC_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] += 1;
                counter += 1;
                break;
            }
            case INS_INC_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] += 1;
                counter += 1;
                break;
            }

            case INS_DEC_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] -= 1;
                counter += 1;
                break;
            }
            case INS_DEC_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] -= 1;
                counter += 1;
                break;
            }
            case INS_DEC_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] -= 1;
                counter += 1;
                break;
            }
            case INS_DEC_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] -= 1;
                counter += 1;
                break;
            }

            case INS_NOT_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] =
                    ~machine->stack.buffer[machine->stack.size - 1].u8[0];
                counter += 1;
                break;
            }
            case INS_NOT_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] =
                    ~machine->stack.buffer[machine->stack.size - 1].u16[0];
                counter += 1;
                break;
            }
            case INS_NOT_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] =
                    ~machine->stack.buffer[machine->stack.size - 1].u32[0];
                counter += 1;
                break;
            }
            case INS_NOT_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] =
                    ~machine->stack.buffer[machine->stack.size - 1].u64[0];
                counter += 1;
                break;
            }

            case INS_NEG_S8:
            {
                machine->stack.buffer[machine->stack.size - 1].s8[0] =
                    -machine->stack.buffer[machine->stack.size - 1].s8[0];
                counter += 1;
                break;
            }
            case INS_NEG_S16:
            {
                machine->stack.buffer[machine->stack.size - 1].s16[0] =
                    -machine->stack.buffer[machine->stack.size - 1].s16[0];
                counter += 1;
                break;
            }
            case INS_NEG_S32:
            {
                machine->stack.buffer[machine->stack.size - 1].s32[0] =
                    -machine->stack.buffer[machine->stack.size - 1].s32[0];
                counter += 1;
                break;
            }
            case INS_NEG_S64:
            {
                machine->stack.buffer[machine->stack.size - 1].s64[0] =
                    -machine->stack.buffer[machine->stack.size - 1].s64[0];
                counter += 1;
                break;
            }
            case INS_NEG_FP32:
            {
                machine->stack.buffer[machine->stack.size - 1].fp32[0] =
                    -machine->stack.buffer[machine->stack.size - 1].fp32[0];
                counter += 1;
                break;
            }
            case INS_NEG_FP64:
            {
                machine->stack.buffer[machine->stack.size - 1].fp64[0] =
                    -machine->stack.buffer[machine->stack.size - 1].fp64[0];
                counter += 1;
                break;
            }

            case INS_OR_I8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] |=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_OR_I16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] |=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_OR_I32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] |=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_OR_I64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] |=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_ORI_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] |=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_ORI_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] |=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_ORI_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] |=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_ORI_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] |=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }

            case INS_XOR_I8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] ^=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_XOR_I16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] ^=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_XOR_I32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] ^=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_XOR_I64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] ^=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_XORI_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] ^=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_XORI_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] ^=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_XORI_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] ^=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_XORI_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] ^=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }

            case INS_AND_I8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] &=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_AND_I16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] &=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_AND_I32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] &=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_AND_I64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] &=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_ANDI_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] &=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] &=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] &=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_ANDI_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] &=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }

            case INS_SHR_I8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] >>=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SHR_I16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] >>=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SHR_I32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] >>=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SHR_I64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] >>=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_SHRI_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] >>=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] >>=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] >>=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_SHRI_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] >>=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }

            case INS_SHL_I8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] <<=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SHL_I16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] <<=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SHL_I32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] <<=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SHL_I64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] <<=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_SHLI_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] <<=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] <<=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] <<=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_SHLI_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] <<=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }

            case INS_ADD_I8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] +=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ADD_I16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] +=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ADD_I32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] +=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ADD_I64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] +=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ADD_FP32:
            {
                machine->stack.buffer[machine->stack.size - 2].fp32[0] +=
                    machine->stack.buffer[machine->stack.size - 1].fp32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_ADD_FP64:
            {
                machine->stack.buffer[machine->stack.size - 2].fp64[0] +=
                    machine->stack.buffer[machine->stack.size - 1].fp64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }

            case INS_ADDI_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] +=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] +=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] +=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_ADDI_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] +=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }
            case INS_ADDI_FP32:
            {
                machine->stack.buffer[machine->stack.size - 1].fp32[0] +=
                    program[counter + 1].immediate.fp32[0];

                counter += 2;
                break;
            }
            case INS_ADDI_FP64:
            {
                machine->stack.buffer[machine->stack.size - 1].fp64[0] +=
                    program[counter + 1].immediate.fp64[0];

                counter += 2;
                break;
            }

            case INS_SUB_I8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] -=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SUB_I16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] -=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SUB_I32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] -=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SUB_I64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] -=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SUB_FP32:
            {
                machine->stack.buffer[machine->stack.size - 2].fp32[0] -=
                    machine->stack.buffer[machine->stack.size - 1].fp32[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SUB_FP64:
            {
                machine->stack.buffer[machine->stack.size - 2].fp64[0] -=
                    machine->stack.buffer[machine->stack.size - 1].fp64[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_SUBI_I8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] -=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] -=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] -=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_SUBI_I64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] -=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }
            case INS_SUBI_FP32:
            {
                machine->stack.buffer[machine->stack.size - 1].fp32[0] -=
                    program[counter + 1].immediate.fp32[0];

                counter += 2;
                break;
            }
            case INS_SUBI_FP64:
            {
                machine->stack.buffer[machine->stack.size - 1].fp64[0] -=
                    program[counter + 1].immediate.fp64[0];

                counter += 2;
                break;
            }

            case INS_MUL_U8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] *=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_MUL_U16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] *=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_MUL_U32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] *=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MUL_U64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] *=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MUL_S8:
            {
                machine->stack.buffer[machine->stack.size - 2].s8[0] *=
                    machine->stack.buffer[machine->stack.size - 1].s8[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MUL_S16:
            {
                machine->stack.buffer[machine->stack.size - 2].s16[0] *=
                    machine->stack.buffer[machine->stack.size - 1].s16[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MUL_S32:
            {
                machine->stack.buffer[machine->stack.size - 2].s32[0] *=
                    machine->stack.buffer[machine->stack.size - 1].s32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MUL_S64:
            {
                machine->stack.buffer[machine->stack.size - 2].s64[0] *=
                    machine->stack.buffer[machine->stack.size - 1].s64[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MUL_FP32:
            {
                machine->stack.buffer[machine->stack.size - 2].fp32[0] *=
                    machine->stack.buffer[machine->stack.size - 1].fp32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MUL_FP64:
            {
                machine->stack.buffer[machine->stack.size - 2].fp64[0] *=
                    machine->stack.buffer[machine->stack.size - 1].fp64[0];

                machine->stack.size -= 1;
                counter += 1;
            }

            case INS_MULI_U8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] *=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_MULI_U16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] *=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_MULI_U32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] *=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_MULI_U64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] *=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }
            case INS_MULI_S8:
            {
                machine->stack.buffer[machine->stack.size - 1].s8[0] *=
                    program[counter + 1].immediate.s8[0];

                counter += 2;
                break;
            }
            case INS_MULI_S16:
            {
                machine->stack.buffer[machine->stack.size - 1].s16[0] *=
                    program[counter + 1].immediate.s16[0];

                counter += 2;
                break;
            }
            case INS_MULI_S32:
            {
                machine->stack.buffer[machine->stack.size - 1].s32[0] *=
                    program[counter + 1].immediate.s32[0];

                counter += 2;
                break;
            }
            case INS_MULI_S64:
            {
                machine->stack.buffer[machine->stack.size - 1].s64[0] *=
                    program[counter + 1].immediate.s64[0];

                counter += 2;
                break;
            }
            case INS_MULI_FP32:
            {
                machine->stack.buffer[machine->stack.size - 1].fp32[0] *=
                    program[counter + 1].immediate.fp32[0];

                counter += 2;
                break;
            }
            case INS_MULI_FP64:
            {
                machine->stack.buffer[machine->stack.size - 1].fp64[0] *=
                    program[counter + 1].immediate.fp64[0];

                counter += 2;
                break;
            }

            case INS_DIV_U8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] /=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_DIV_U16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] /=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_DIV_U32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] /=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_DIV_U64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] /=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_DIV_S8:
            {
                machine->stack.buffer[machine->stack.size - 2].s8[0] /=
                    machine->stack.buffer[machine->stack.size - 1].s8[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_DIV_S16:
            {
                machine->stack.buffer[machine->stack.size - 2].s16[0] /=
                    machine->stack.buffer[machine->stack.size - 1].s16[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_DIV_S32:
            {
                machine->stack.buffer[machine->stack.size - 2].s32[0] /=
                    machine->stack.buffer[machine->stack.size - 1].s32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_DIV_S64:
            {
                machine->stack.buffer[machine->stack.size - 2].s64[0] /=
                    machine->stack.buffer[machine->stack.size - 1].s64[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_DIV_FP32:
            {
                machine->stack.buffer[machine->stack.size - 2].fp32[0] /=
                    machine->stack.buffer[machine->stack.size - 1].fp32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_DIV_FP64:
            {
                machine->stack.buffer[machine->stack.size - 2].fp64[0] /=
                    machine->stack.buffer[machine->stack.size - 1].fp64[0];

                machine->stack.size -= 1;
                counter += 1;
            }

            case INS_DIVI_U8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] /=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] /=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] /=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_DIVI_U64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] /=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }
            case INS_DIVI_S8:
            {
                machine->stack.buffer[machine->stack.size - 1].s8[0] /=
                    program[counter + 1].immediate.s8[0];

                counter += 2;
                break;
            }
            case INS_DIVI_S16:
            {
                machine->stack.buffer[machine->stack.size - 1].s16[0] /=
                    program[counter + 1].immediate.s16[0];

                counter += 2;
                break;
            }
            case INS_DIVI_S32:
            {
                machine->stack.buffer[machine->stack.size - 1].s32[0] /=
                    program[counter + 1].immediate.s32[0];

                counter += 2;
                break;
            }
            case INS_DIVI_S64:
            {
                machine->stack.buffer[machine->stack.size - 1].s64[0] /=
                    program[counter + 1].immediate.s64[0];

                counter += 2;
                break;
            }
            case INS_DIVI_FP32:
            {
                machine->stack.buffer[machine->stack.size - 1].fp32[0] /=
                    program[counter + 1].immediate.fp32[0];

                counter += 2;
                break;
            }
            case INS_DIVI_FP64:
            {
                machine->stack.buffer[machine->stack.size - 1].fp64[0] /=
                    program[counter + 1].immediate.fp64[0];

                counter += 2;
                break;
            }

            case INS_MOD_U8:
            {
                machine->stack.buffer[machine->stack.size - 2].u8[0] %=
                    machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_MOD_U16:
            {
                machine->stack.buffer[machine->stack.size - 2].u16[0] %=
                    machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->stack.size -= 1;
                counter += 1;
                break;
            }
            case INS_MOD_U32:
            {
                machine->stack.buffer[machine->stack.size - 2].u32[0] %=
                    machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MOD_U64:
            {
                machine->stack.buffer[machine->stack.size - 2].u64[0] %=
                    machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MOD_S8:
            {
                machine->stack.buffer[machine->stack.size - 2].s8[0] %=
                    machine->stack.buffer[machine->stack.size - 1].s8[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MOD_S16:
            {
                machine->stack.buffer[machine->stack.size - 2].s16[0] %=
                    machine->stack.buffer[machine->stack.size - 1].s16[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MOD_S32:
            {
                machine->stack.buffer[machine->stack.size - 2].s32[0] %=
                    machine->stack.buffer[machine->stack.size - 1].s32[0];

                machine->stack.size -= 1;
                counter += 1;
            }
            case INS_MOD_S64:
            {
                machine->stack.buffer[machine->stack.size - 2].s64[0] %=
                    machine->stack.buffer[machine->stack.size - 1].s64[0];

                machine->stack.size -= 1;
                counter += 1;
            }

            case INS_MODI_U8:
            {
                machine->stack.buffer[machine->stack.size - 1].u8[0] %=
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_MODI_U16:
            {
                machine->stack.buffer[machine->stack.size - 1].u16[0] %=
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_MODI_U32:
            {
                machine->stack.buffer[machine->stack.size - 1].u32[0] %=
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_MODI_U64:
            {
                machine->stack.buffer[machine->stack.size - 1].u64[0] %=
                    program[counter + 1].immediate.u64[0];

                counter += 2;
                break;
            }
            case INS_MODI_S8:
            {
                machine->stack.buffer[machine->stack.size - 1].s8[0] %=
                    program[counter + 1].immediate.s8[0];

                counter += 2;
                break;
            }
            case INS_MODI_S16:
            {
                machine->stack.buffer[machine->stack.size - 1].s16[0] %=
                    program[counter + 1].immediate.s16[0];

                counter += 2;
                break;
            }
            case INS_MODI_S32:
            {
                machine->stack.buffer[machine->stack.size - 1].s32[0] %=
                    program[counter + 1].immediate.s32[0];

                counter += 2;
                break;
            }
            case INS_MODI_S64:
            {
                machine->stack.buffer[machine->stack.size - 1].s64[0] %=
                    program[counter + 1].immediate.s64[0];

                counter += 2;
                break;
            }

            case INS_CMP_U8:
            {
                U8 a = machine->stack.buffer[machine->stack.size - 2].u8[0];
                U8 b = machine->stack.buffer[machine->stack.size - 1].u8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U16:
            {
                U16 a = machine->stack.buffer[machine->stack.size - 2].u16[0];
                U16 b = machine->stack.buffer[machine->stack.size - 1].u16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U32:
            {
                U32 a = machine->stack.buffer[machine->stack.size - 2].u32[0];
                U32 b = machine->stack.buffer[machine->stack.size - 1].u32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_U64:
            {
                U64 a = machine->stack.buffer[machine->stack.size - 2].u64[0];
                U64 b = machine->stack.buffer[machine->stack.size - 1].u64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S8:
            {
                S8 a = machine->stack.buffer[machine->stack.size - 2].s8[0];
                S8 b = machine->stack.buffer[machine->stack.size - 1].s8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S16:
            {
                S16 a = machine->stack.buffer[machine->stack.size - 2].s16[0];
                S16 b = machine->stack.buffer[machine->stack.size - 1].s16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S32:
            {
                S32 a = machine->stack.buffer[machine->stack.size - 2].s32[0];
                S32 b = machine->stack.buffer[machine->stack.size - 1].s32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_S64:
            {
                S64 a = machine->stack.buffer[machine->stack.size - 2].s64[0];
                S64 b = machine->stack.buffer[machine->stack.size - 1].s64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_FP32:
            {
                FP32 a = machine->stack.buffer[machine->stack.size - 2].fp32[0];
                FP32 b = machine->stack.buffer[machine->stack.size - 1].fp32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMP_FP64:
            {
                FP64 a = machine->stack.buffer[machine->stack.size - 2].fp64[0];
                FP64 b = machine->stack.buffer[machine->stack.size - 1].fp64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 1;
                break;
            }
            case INS_CMPI_U8:
            {
                U8 a = machine->stack.buffer[machine->stack.size - 2].u8[0];
                U8 b = program[counter + 1].immediate.u8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U16:
            {
                U16 a = machine->stack.buffer[machine->stack.size - 2].u16[0];
                U16 b = program[counter + 1].immediate.u16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U32:
            {
                U32 a = machine->stack.buffer[machine->stack.size - 2].u32[0];
                U32 b = program[counter + 1].immediate.u32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U64:
            {
                U64 a = machine->stack.buffer[machine->stack.size - 2].u64[0];
                U64 b = program[counter + 1].immediate.u64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S8:
            {
                S8 a = machine->stack.buffer[machine->stack.size - 2].s8[0];
                S8 b = program[counter + 1].immediate.s8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S16:
            {
                S16 a = machine->stack.buffer[machine->stack.size - 2].s16[0];
                S16 b = program[counter + 1].immediate.s16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S32:
            {
                S32 a = machine->stack.buffer[machine->stack.size - 2].s32[0];
                S32 b = program[counter + 1].immediate.s32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S64:
            {
                S64 a = machine->stack.buffer[machine->stack.size - 2].s64[0];
                S64 b = program[counter + 1].immediate.s64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP32:
            {
                FP32 a = machine->stack.buffer[machine->stack.size - 2].fp32[0];
                FP32 b = program[counter + 1].immediate.fp32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP64:
            {
                FP64 a = machine->stack.buffer[machine->stack.size - 2].fp64[0];
                FP64 b = program[counter + 1].immediate.fp64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
            }
            case INS_JEQ:
            {
                if (machine->cmp == 0)
                {
                    counter = program[counter + 1].immediate.u64[0];
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
                    counter = program[counter + 1].immediate.u64[0];
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
                    counter = program[counter + 1].immediate.u64[0];
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
                    counter = program[counter + 1].immediate.u64[0];
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
                    counter = program[counter + 1].immediate.u64[0];
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
                    counter = program[counter + 1].immediate.u64[0];
                }
                else
                {
                    counter += 2;
                }

                break;
            }
            case INS_JMP:
            {
                counter = program[counter + 1].immediate.u64[0];
                break;
            }
            /*
            case INS_NATIVE_CALL:
            {
                U64 number_of_parameters =
                    (program + counter + 3)->immediate.u64[0];

                if (machine->size + ((number_of_parameters + 1) * 2) >
                    machine->capacity)
                {
                    U8* buffer =
                        (U8*)malloc(sizeof(U8) * ((machine->size + 1) << 1));

                    if (!buffer)
                    {
                        Panic(Memory_Error);
                    }

                    memcpy(buffer, machine->stack, machine->size);

                    machine->stack = buffer;
                }

                void* function_pointer =
                    (void*)(program[counter + 1].immediate.u64[0]);

                U64 return_type = program[counter + 2].immediate.u32[0];
                U64 return_size = program[counter + 2].imm_i32[1];

                U8* ptr = machine->stack + machine->size;

                size_t amount_to_shrink = 0;

                for (size_t it = number_of_parameters; it > 0; it--)
                {
                    U64 param_type =
                        (program + counter + it + 3)->immediate.u32[0];
                    U64 param_size = (program + counter + it + 3)->imm_i32[1];

                    Call_Type_And_Value* param =
                        (Call_Type_And_Value*)(machine->stack + machine->size +
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
                    (Call_Type_And_Value*)(machine->stack + machine->size);

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
                    (size_t)((machine->stack + machine->size) - (ptr));
                counter += 4 + number_of_parameters;
                break;
            }
            */
            default:
            {
                assert(False && "Invalid Instruction");
            }
        }
    }
}
