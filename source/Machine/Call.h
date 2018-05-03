#ifndef CALL_H
#define CALL_H

#include <Utility/Standard.h>

typedef enum Call_Type
{
    Call_Integral_1,
    Call_Integral_2,
    Call_Integral_4,
    Call_Integral_8,
    Call_Float_4,
    Call_Float_8,
    Call_Other,
    Call_Void,
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

extern size_t Native_Call(U64 return_type, Call_Value return_value,
                          U64 number_of_parameters,
                          Call_Type_And_Value* parameters);

#endif
