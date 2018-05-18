#include <Machine/Execution.h>

#define GROW(MACHINE, LOCATION, SIZE, TYPE)                             \
    {                                                                   \
        if (MACHINE->LOCATION.size + SIZE > MACHINE->LOCATION.capacity) \
        {                                                               \
            size_t capacity = (MACHINE->LOCATION.size + SIZE) << 1;     \
            TYPE* buffer = (TYPE*)malloc(sizeof(TYPE) * capacity);      \
                                                                        \
            if (!buffer)                                                \
            {                                                           \
                Panic(Memory_Error);                                    \
            }                                                           \
                                                                        \
            memcpy(buffer, MACHINE->LOCATION.buffer,                    \
                   sizeof(TYPE) * MACHINE->LOCATION.size);              \
                                                                        \
            MACHINE->LOCATION.buffer = buffer;                          \
            MACHINE->LOCATION.capacity = capacity;                      \
        }                                                               \
    }

#define MACHINE_GROW_STACK(MACHINE, SIZE) GROW(MACHINE, stack, SIZE, Value)

#define MACHINE_GROW_LOCAL(MACHINE, SIZE) GROW(MACHINE, local, SIZE, Value*)

#define MACHINE_GROW_GLOBAL(MACHINE, SIZE) GROW(MACHINE, global, SIZE, PTR)

void execute(Program* program, Machine* machine)
{
    size_t counter = 0;

    for (Boolean step = True; step;)
    {
        switch (program[counter].instruction)
        {
            case INS_RETURN:
            {
                step = False;
                break;
            }

            case INS_ALLOC:
            {
                size_t size = program[counter + 1].immediate.u64[0];

                size = (size & 0x7) ? (size | 0x7) + 1 : size;

                Value* address = (Value*)malloc(size);
                MACHINE_GROW_LOCAL(machine, 1);
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

                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u8[0] =
                    *(U8*)(address);

                counter += 2;
                break;
            }

            case INS_LDG_16:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                PTR address = machine->global.buffer[index];

                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u16[0] =
                    *(U16*)(address);

                counter += 2;
                break;
            }

            case INS_LDG_32:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                PTR address = machine->global.buffer[index];

                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u32[0] =
                    *(U32*)(address);

                counter += 2;
                break;
            }

            case INS_LDG_64:
            {
                size_t index = program[counter + 1].immediate.u64[0];
                PTR address = machine->global.buffer[index];

                MACHINE_GROW_STACK(machine, 1);

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

                MACHINE_GROW_STACK(machine, 1);

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

                MACHINE_GROW_STACK(machine, 1);

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

                MACHINE_GROW_STACK(machine, 1);

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

                MACHINE_GROW_STACK(machine, 1);

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

                MACHINE_GROW_STACK(machine, 1);

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

                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].ptr[0] =
                    (PTR)(&address[high]);

                counter += 2;
                break;
            }

            case INS_LDI_8:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u8[0] =
                    *(U8*)(address);

                counter += 2;
                break;
            }

            case INS_LDI_16:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u16[0] =
                    *(U16*)(address);

                counter += 2;
                break;
            }

            case INS_LDI_32:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u32[0] =
                    *(U32*)(address);

                counter += 2;
                break;
            }

            case INS_LDI_64:
            {
                size_t address = program[counter + 1].immediate.u64[0];

                MACHINE_GROW_STACK(machine, 1);

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
                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u8[0] =
                    program[counter + 1].immediate.u8[0];

                counter += 2;
                break;
            }
            case INS_PUSH_16:
            {
                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u16[0] =
                    program[counter + 1].immediate.u16[0];

                counter += 2;
                break;
            }
            case INS_PUSH_32:
            {
                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size++].u32[0] =
                    program[counter + 1].immediate.u32[0];

                counter += 2;
                break;
            }
            case INS_PUSH_64:
            {
                MACHINE_GROW_STACK(machine, 1);

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

            case INS_DUP:
            {
                MACHINE_GROW_STACK(machine, 1);

                machine->stack.buffer[machine->stack.size] =
                    machine->stack.buffer[machine->stack.size - 1];

                machine->stack.size += 1;
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

            case INS_SGNEXT_I8_I16:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U8 src = operand->u8[0];
                operand->u16[0] = (src >> 7) ? 0xFFFF : 0;
                operand->u8[0] = src;

                counter += 1;
                break;
            }

            case INS_SGNEXT_I8_I32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U8 src = operand->u8[0];
                operand->u32[0] = (src >> 7) ? 0xFFFFFFFF : 0;
                operand->u8[0] = src;

                counter += 1;
                break;
            }

            case INS_SGNEXT_I8_I64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U8 src = operand->u8[0];
                operand->u64[0] = (src >> 7) ? 0xFFFFFFFFFFFFFFFFULL : 0;
                operand->u8[0] = src;

                counter += 1;
                break;
            }

            case INS_SGNEXT_I16_I32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U16 src = operand->u16[0];
                operand->u32[0] = (src >> 15) ? 0xFFFFFFFF : 0;
                operand->u16[0] = src;

                counter += 1;
                break;
            }

            case INS_SGNEXT_I16_I64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U16 src = operand->u16[0];
                operand->u64[0] = (src >> 15) ? 0xFFFFFFFFFFFFFFFFULL : 0;
                operand->u16[0] = src;

                counter += 1;
                break;
            }

            case INS_SGNEXT_I32_I64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U32 src = operand->u32[0];
                operand->u64[0] = (src >> 31) ? 0xFFFFFFFFFFFFFFFFULL : 0;
                operand->u32[0] = src;

                counter += 1;
                break;
            }

            case INS_ZEREXT_I8_I16:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U8 src = operand->u8[0];
                operand->u16[0] = 0;
                operand->u8[0] = src;

                counter += 1;
                break;
            }

            case INS_ZEREXT_I8_I32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U8 src = operand->u8[0];
                operand->u32[0] = 0;
                operand->u8[0] = src;

                counter += 1;
                break;
            }

            case INS_ZEREXT_I8_I64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U8 src = operand->u8[0];
                operand->u64[0] = 0;
                operand->u8[0] = src;

                counter += 1;
                break;
            }

            case INS_ZEREXT_I16_I32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U16 src = operand->u16[0];
                operand->u32[0] = 0;
                operand->u16[0] = src;

                counter += 1;
                break;
            }

            case INS_ZEREXT_I16_I64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U16 src = operand->u16[0];
                operand->u64[0] = 0;
                operand->u16[0] = src;

                counter += 1;
                break;
            }

            case INS_ZEREXT_I32_I64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                U32 src = operand->u32[0];
                operand->u64[0] = 0;
                operand->u32[0] = src;

                counter += 1;
                break;
            }

            case INS_CONV_U8_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->u8[0]);

                counter += 1;
                break;
            }

            case INS_CONV_U16_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->u16[0]);

                counter += 1;
                break;
            }

            case INS_CONV_U32_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->u32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_U64_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->u64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S8_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->s8[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S16_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->s16[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S32_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->s32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S64_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->s64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_U8:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u8[0] = (U8)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_U16:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u16[0] = (U16)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_U32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u32[0] = (U32)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_U64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u64[0] = (U64)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_S8:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s8[0] = (S8)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_S16:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s16[0] = (S16)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_S32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s32[0] = (S32)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_S64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s64[0] = (S64)(operand->fp32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_U8_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->u8[0]);

                counter += 1;
                break;
            }

            case INS_CONV_U16_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->u16[0]);

                counter += 1;
                break;
            }

            case INS_CONV_U32_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->u32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_U64_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->u64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S8_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->s8[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S16_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->s16[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S32_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->s32[0]);

                counter += 1;
                break;
            }

            case INS_CONV_S64_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP64)(operand->s64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_U8:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u8[0] = (U8)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_U16:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u16[0] = (U16)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_U32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u32[0] = (U32)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_U64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->u64[0] = (U64)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_S8:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s8[0] = (S8)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_S16:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s16[0] = (S16)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_S32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s32[0] = (S32)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_S64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->s64[0] = (S64)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP64_FP32:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp32[0] = (FP32)(operand->fp64[0]);

                counter += 1;
                break;
            }

            case INS_CONV_FP32_FP64:
            {
                Value* operand =
                    &machine->stack.buffer[machine->stack.size - 1];

                operand->fp64[0] = (FP32)(operand->fp32[0]);

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
                U8 a = machine->stack.buffer[machine->stack.size - 1].u8[0];
                U8 b = program[counter + 1].immediate.u8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U16:
            {
                U16 a = machine->stack.buffer[machine->stack.size - 1].u16[0];
                U16 b = program[counter + 1].immediate.u16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U32:
            {
                U32 a = machine->stack.buffer[machine->stack.size - 1].u32[0];
                U32 b = program[counter + 1].immediate.u32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_U64:
            {
                U64 a = machine->stack.buffer[machine->stack.size - 1].u64[0];
                U64 b = program[counter + 1].immediate.u64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S8:
            {
                S8 a = machine->stack.buffer[machine->stack.size - 1].s8[0];
                S8 b = program[counter + 1].immediate.s8[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S16:
            {
                S16 a = machine->stack.buffer[machine->stack.size - 1].s16[0];
                S16 b = program[counter + 1].immediate.s16[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S32:
            {
                S32 a = machine->stack.buffer[machine->stack.size - 1].s32[0];
                S32 b = program[counter + 1].immediate.s32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_S64:
            {
                S64 a = machine->stack.buffer[machine->stack.size - 1].s64[0];
                S64 b = program[counter + 1].immediate.s64[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP32:
            {
                FP32 a = machine->stack.buffer[machine->stack.size - 1].fp32[0];
                FP32 b = program[counter + 1].immediate.fp32[0];

                machine->cmp = (a == b) ? 0 : (a < b) ? -1 : 1;

                counter += 2;
                break;
            }
            case INS_CMPI_FP64:
            {
                FP64 a = machine->stack.buffer[machine->stack.size - 1].fp64[0];
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
            case INS_NATIVE_CALL:
            {
                PTR function_pointer = program[counter + 1].immediate.ptr[0];

                U64 number_of_parameters =
                    program[counter + 2].immediate.u64[0];

                Native_Call(
                    function_pointer,
                    *(Call_Type_And_Value*)(machine->stack.buffer +
                                            machine->stack.size -
                                            ((number_of_parameters + 1) * 2)),
                    number_of_parameters,
                    (Call_Type_And_Value*)(machine->stack.buffer +
                                           machine->stack.size -
                                           ((number_of_parameters)*2)));

                machine->stack.size -= (number_of_parameters + 1) * 2;
                counter += 3;
                break;
            }
            case INS_CALL:
            {
                Program* callee = program[counter + 1].immediate.ptr[0];
                execute(callee, machine);

                counter += 2;
                break;
            }
            default:
            {
                assert(False && "Invalid Instruction");
            }
        }
    }
}
