#ifndef NATIVE_CALL_H
#define NATIVE_CALL_H

#include <Utility/Standard.h>

typedef enum Native_Call_Type
{
    Native_Call_Integer,
    Native_Call_Float,
    Native_Call_Struct,
} Native_Call_Type;

typedef struct Native_Call_Value
{
    u32 type;
    u32 size;
    void* value;
} Native_Call_Value;

extern void Native_Call(void* function_pointer,
                        Native_Call_Value return_pointer,
                        u64 number_of_parameters,
                        Native_Call_Value* parameters);

#endif
