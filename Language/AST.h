#ifndef AST_H
#define AST_H

#include <Language/Lexer.h>
#include <Language/Types.h>
#include <Support/Stack.h>
#include <Support/String.h>

typedef enum AST_ID
{
    AST_PROGRAM,

    AST_DECLARE,
    AST_FUNCTION,

    AST_BLOCK,
    AST_WHILE,
    AST_IF,
    AST_ASSIGN,

    AST_TUPLE,
    AST_BINARY,
    AST_UNARY,
    AST_INTEGER_LITERAL,
    AST_FLOAT_LITERAL,
    AST_STRING_LITERAL,
    AST_STORAGE,
    AST_ADDRESS,
    AST_CALL,
} AST_ID;

typedef struct AST
{
    AST_ID AST_id;
} AST;

typedef struct AST_Expression
{
    AST_ID AST_id;
    Type* type;
} AST_Expression;

typedef struct AST_Program
{
    AST_ID AST_id;

    Stack statements;

    size_t data;
} AST_Program;

typedef struct AST_Type
{
    AST_ID AST_id;
    String id;
    Type* type;
} AST_Type;

typedef struct AST_Declare
{
    AST_ID AST_id;

    Boolean is_const;

    Stack ids;
    Type* type;
    AST_Expression* initializer;
} AST_Declare;

typedef struct AST_Function
{
    AST_ID AST_id;

    String id;

    Stack in_ids;
    Stack in_types;

    Stack out_ids;
    Stack out_types;

    AST* body;

    size_t data;
} AST_Function;

typedef struct AST_Block
{
    AST_ID AST_id;

    Stack statements;
} AST_Block;

typedef struct AST_While
{
    AST_ID AST_id;

    AST_Expression* condition;
    AST* then;
} AST_While;

typedef struct AST_If
{
    AST_ID AST_id;

    AST_Expression* condition;
    AST* then;
    AST* else_;
} AST_If;

typedef struct AST_Assign
{
    AST_ID AST_id;

    AST_Expression* lhs;
    AST_Expression* rhs;
} AST_Assign;

typedef struct AST_Tuple
{
    AST_ID AST_id;
    Type* type;

    Stack fields;
} AST_Tuple;

typedef struct AST_Binary
{
    AST_ID AST_id;
    Type* type;

    Tag op;
    AST_Expression* lhs;
    AST_Expression* rhs;
} AST_Binary;

typedef struct AST_Unary
{
    AST_ID AST_id;
    Type* type;

    Tag op;
    AST_Expression* expression;
} AST_Unary;

typedef struct AST_Integer_Literal
{
    AST_ID AST_id;
    Type* type;

    U64 value;
} AST_Integer_Literal;

typedef struct AST_Real_Literal
{
    AST_ID AST_id;
    Type* type;

    FP64 value;
} AST_Real_Literal;

typedef struct AST_String_Literal
{
    AST_ID AST_id;
    Type* type;

    String value;
} AST_String_Literal;

typedef struct AST_Storage
{
    AST_ID AST_id;
    Type* type;

    size_t offset;
    Boolean global;
} AST_Storage;

typedef struct AST_Address
{
    AST_ID AST_id;
    Type* type;

    AST_Expression* base;
    AST_Expression* offset;
} AST_Address;

typedef struct AST_Call
{
    AST_ID AST_id;
    Type* type;

    AST_Expression* callee;
    AST_Expression* arguments;
} AST_Call;

AST_Program* AST_Program_create();
AST_Block* AST_Block_create();
AST_Declare* AST_Declare_create();
AST_Function* AST_Function_create();
AST_While* AST_While_create();
AST_If* AST_If_create();
AST_Assign* AST_Assign_create();
AST_Tuple* AST_Tuple_create();
AST_Binary* AST_Binary_create();
AST_Unary* AST_Unary_create();
AST_Integer_Literal* AST_Integer_Literal_create();
AST_Real_Literal* AST_Real_Literal_create();
AST_String_Literal* AST_String_Literal_create();
AST_Storage* AST_Storage_create();
AST_Address* AST_Address_create();
AST_Call* AST_Call_create();

void AST_destroy(AST* self);
void AST_print(AST* self);

#endif
