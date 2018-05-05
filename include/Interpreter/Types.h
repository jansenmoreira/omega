#ifndef TYPES_H
#define TYPES_H

#include <Structures/Stack.h>
#include <Utility/String.h>

typedef enum Type_ID
{
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_ARRAY,
    TYPE_POINTER,
    TYPE_TUPLE,
    TYPE_STRUCT,
    TYPE_FUNCTION
} Type_ID;

typedef struct Type
{
    Type_ID type_id;
} Type;

typedef struct Type_Float
{
    Type_ID type_id;

    size_t size;
} Type_Float;

typedef struct Type_Integer
{
    Type_ID type_id;

    size_t size;
    int is_signed;
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

    Stack fields;
    Stack ids;
} Type_Struct;

typedef struct Type_Function
{
    Type_ID type_id;

    Stack params;
    Type* return_type;
} Type_Function;

Type_Integer* Type_Integer_create(size_t size, int is_signed);
Type_Float* Type_Float_create(size_t size);
Type_Array* Type_Array_create(Type* type, size_t size);
Type_Pointer* Type_Pointer_create(Type* type);
Type_Struct* Type_Struct_create();
Type_Tuple* Type_Tuple_create();
Type_Function* Type_Function_create();

Type* Type_Copy(Type* type);

void Type_destroy(Type* type);

size_t Type_size(Type* type);

Boolean Type_equal(Type* a, Type* b);

void Type_print(Type* type);

void Type_print_value(Type* type, void* value);

#endif
