#ifndef CALL_H
#define CALL_H

#include <Support/Standard.h>

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

typedef struct Call_Type_And_Value
{
    U64 type;
    Value value;
} Call_Type_And_Value;

extern void Native_Call(void* function, Call_Type_And_Value ret,
                        U64 number_of_params, Call_Type_And_Value* params);

#endif
