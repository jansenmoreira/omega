#ifndef CALL_H
#define CALL_H

#include <Utility/Standard.h>

typedef enum Call_Type
{
    Call_Type_I8,
    Call_Type_I16,
    Call_Type_I32,
    Call_Type_I64,
    Call_Type_FP32,
    Call_Type_FP64,
    Call_Type_Struct,
    Call_Type_Void,
} Call_Type;

typedef union Call_Value {
    U64 u64;
    U32 u32[2];
    U16 u16[4];
    U8 u8[8];
    FP64 fp64;
    FP32 fp32[2];
    PTR ptr;
} Call_Value;

typedef struct Call_Type_And_Value
{
    U64 type;
    Call_Value value;
} Call_Type_And_Value;

extern void Native_Call(void* function, Call_Type_And_Value ret,
                        U64 number_of_params, Call_Type_And_Value* params);

#endif
