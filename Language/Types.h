#ifndef TYPES_H
#define TYPES_H

#include <Support/Stack.h>
#include <Support/String.h>

typedef enum Type_ID
{
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_ARRAY,
    TYPE_POINTER,
    TYPE_TUPLE,
    TYPE_STRUCT,
    TYPE_FUNCTION,
    TYPE_TYPE,
} Type_ID;

typedef struct Type
{
    Type_ID type_id;
} Type;

typedef struct Type_Type
{
    Type_ID type_id;
    Type* type;
} Type_Type;

typedef struct Type_Float
{
    Type_ID type_id;

    size_t size;
} Type_Float;

typedef struct Type_Integer
{
    Type_ID type_id;

    size_t size;
    Boolean is_signed;
} Type_Integer;

typedef struct Type_Array
{
    Type_ID type_id;

    Type* type;
    size_t size;
} Type_Array;

typedef struct Type_Pointer
{
    Type_ID type_id;

    Type* type;
} Type_Pointer;

typedef struct Type_Tuple
{
    Type_ID type_id;

    Stack fields;
} Type_Tuple;

typedef struct Type_Struct
{
    Type_ID type_id;

    String id;
    Stack field_types;
    Stack field_ids;
} Type_Struct;

typedef struct Type_Function
{
    Type_ID type_id;

    Stack params;
    Type* return_type;
} Type_Function;

Type* type_u8;
Type* type_u16;
Type* type_u32;
Type* type_u64;
Type* type_s8;
Type* type_s16;
Type* type_s32;
Type* type_s64;
Type* type_fp32;
Type* type_fp64;
Type* type_type;

Type_Type* Type_Type_create();
Type_Array* Type_Array_create();
Type_Pointer* Type_Pointer_create();
Type_Struct* Type_Struct_create();
Type_Tuple* Type_Tuple_create();
Type_Function* Type_Function_create();

Type* Type_Copy(Type* type);

void Type_destroy(Type* type);

size_t Type_size(Type* type);

Boolean Type_equal(Type* a, Type* b);

#endif
