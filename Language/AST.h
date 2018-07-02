#ifndef AST_H
#define AST_H

#include <Language/Lexer.h>
#include <Support/Stack.h>
#include <Support/String.h>

typedef enum AST_ID
{
    AST_PROGRAM,
    AST_IMPORT,
    AST_DECLARE,
    AST_STRUCT,
    AST_MAP,
    AST_FUNCTION,
    AST_WHILE,
    AST_IF,
    AST_SIZE,
    AST_ASSIGN,
    AST_BINARY,
    AST_UNARY,
    AST_INTEGER_LITERAL,
    AST_REAL_LITERAL,
    AST_STRING_LITERAL,
    AST_TUPLE,
    AST_REFERENCE,
    AST_SUBSCRIPTING,
    AST_MEMBER,
    AST_CALL,
    AST_BLOCK
} AST_ID;

typedef struct AST
{
    AST_ID AST_id;
} AST;

typedef struct AST_Block
{
    AST_ID AST_id;

    Stack statements;
} AST_Block;

typedef struct AST_Program
{
    AST_ID AST_id;

    Stack statements;
} AST_Program;

typedef struct AST_Import
{
    AST_ID AST_id;

    String path;
} AST_Import;

typedef struct AST_Declare
{
    AST_ID AST_id;

    Boolean is_const;

    Stack ids;
    AST* type;
    AST* initializer;
} AST_Declare;

typedef struct AST_Struct
{
    AST_ID AST_id;

    String id;
    Stack ids;
    Stack types;
} AST_Struct;

typedef struct AST_Function
{
    AST_ID AST_id;

    Stack in_ids;
    AST* in_type;

    Stack out_ids;
    AST* out_type;

    AST* body;
} AST_Function;

typedef struct AST_While
{
    AST_ID AST_id;

    AST* condition;
    AST* then;
} AST_While;

typedef struct AST_If
{
    AST_ID AST_id;

    AST* condition;
    AST* then;
    AST* else_;
} AST_If;

typedef struct AST_Size
{
    AST_ID AST_id;

    AST* expression;
} AST_Size;

typedef struct AST_Assign
{
    AST_ID AST_id;

    AST* lhs;
    AST* rhs;
} AST_Assign;

typedef struct AST_Binary
{
    AST_ID AST_id;

    Tag op;
    AST* lhs;
    AST* rhs;
} AST_Binary;

typedef struct AST_Unary
{
    AST_ID AST_id;

    Tag op;
    AST* expression;
} AST_Unary;

typedef struct AST_Integer_Literal
{
    AST_ID AST_id;

    String value;
} AST_Integer_Literal;

typedef struct AST_Real_Literal
{
    AST_ID AST_id;

    String value;
} AST_Real_Literal;

typedef struct AST_String_Literal
{
    AST_ID AST_id;

    String value;
} AST_String_Literal;

typedef struct AST_Map
{
    AST_ID AST_id;

    AST* params;
    AST* result;
} AST_Map;

typedef struct AST_Tuple
{
    AST_ID AST_id;

    Stack fields;
} AST_Tuple;

typedef struct AST_Reference
{
    AST_ID AST_id;

    String id;
} AST_Reference;

typedef struct AST_Subscripting
{
    AST_ID AST_id;

    AST* lhs;
    AST* rhs;
} AST_Subscripting;

typedef struct AST_Member
{
    AST_ID AST_id;

    AST* lhs;
    String id;
} AST_Member;

typedef struct AST_Call
{
    AST_ID AST_id;

    AST* callee;
    AST* argument;
} AST_Call;

AST_Program* AST_Program_create();
AST_Block* AST_Block_create();
AST_Import* AST_Import_create();
AST_Declare* AST_Declare_create();
AST_Struct* AST_Struct_create();
AST_Function* AST_Function_create();
AST_Map* AST_Map_create();
AST_While* AST_While_create();
AST_If* AST_If_create();
AST_Size* AST_Size_create();
AST_Assign* AST_Assign_create();
AST_Binary* AST_Binary_create();
AST_Unary* AST_Unary_create();
AST_Integer_Literal* AST_Integer_Literal_create();
AST_Real_Literal* AST_Real_Literal_create();
AST_String_Literal* AST_String_Literal_create();
AST_Tuple* AST_Tuple_create();
AST_Reference* AST_Reference_create();
AST_Subscripting* AST_Subscripting_create();
AST_Member* AST_Member_create();
AST_Call* AST_Call_create();

void AST_destroy(AST* self);
void AST_print(AST* self);

#endif
