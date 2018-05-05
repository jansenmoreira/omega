#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <Interpreter/Types.h>
#include <Structures/Stack.h>
#include <Utility/String.h>

typedef enum Expression_ID
{
    EXPRESSION_CAST,
    EXPRESSION_SIZE,
    EXPRESSION_TYPE,
    EXPRESSION_ASSIGN,
    EXPRESSION_BINARY,
    EXPRESSION_UNARY,
    EXPRESSION_INTEGER_LITERAL,
    EXPRESSION_REAL_LITERAL,
    EXPRESSION_STRING_LITERAL,
    EXPRESSION_TUPLE,
    EXPRESSION_REFERENCE,
    EXPRESSION_SUBSCRIPTING,
    EXPRESSION_MEMBER,
    EXPRESSION_CALL
} Expression_ID;

typedef struct Expression
{
    Expression_ID expression_id;
} Expression;

typedef struct Expression_Cast
{
    Expression_ID expression_id;

    Type* type;
    Expression* expression;
} Expression_Cast;

typedef struct Expression_Size
{
    Expression_ID expression_id;

    Type* type;
} Expression_Size;

typedef struct Expression_Type
{
    Expression_ID expression_id;

    Expression* expression;
} Expression_Type;

typedef struct Expression_Assign
{
    Expression_ID expression_id;

    Expression* lhs;
    Expression* rhs;
} Expression_Assign;

typedef struct Expression_Binary
{
    Expression_ID expression_id;

    int op;
    Expression* lhs;
    Expression* rhs;
} Expression_Binary;

typedef struct Expression_Unary
{
    Expression_ID expression_id;

    int op;
    Expression* expression;
} Expression_Unary;

typedef struct Expression_Integer_Literal
{
    Expression_ID expression_id;

    String value;
} Expression_Integer_Literal;

typedef struct Expression_Real_Literal
{
    Expression_ID expression_id;

    String value;
} Expression_Real_Literal;

typedef struct Expression_String_Literal
{
    Expression_ID expression_id;

    String value;
} Expression_String_Literal;

typedef struct Expression_Tuple
{
    Expression_ID expression_id;

    Stack fields;
} Expression_Tuple;

typedef struct Expression_Reference
{
    Expression_ID expression_id;

    String id;
} Expression_Reference;

typedef struct Expression_Subscripting
{
    Expression_ID expression_id;

    Expression* lhs;
    Expression* rhs;
} Expression_Subscripting;

typedef struct Expression_Member
{
    Expression_ID expression_id;

    Expression* lhs;
    Expression* rhs;
} Expression_Member;

typedef struct Expression_Call
{
    Expression_ID expression_id;

    Expression* callee;
    Stack arguments;
} Expression_Call;

Expression_Cast* Expression_Cast_create();
Expression_Size* Expression_Size_create();
Expression_Type* Expression_Type_create();
Expression_Assign* Expression_Assign_create();
Expression_Binary* Expression_Binary_create();
Expression_Unary* Expression_Unary_create();
Expression_Integer_Literal* Expression_Integer_Literal_create();
Expression_Real_Literal* Expression_Real_Literal_create();
Expression_String_Literal* Expression_String_Literal_create();
Expression_Tuple* Expression_Tuple_create();
Expression_Reference* Expression_Reference_create();
Expression_Subscripting* Expression_Subscripting_create();
Expression_Member* Expression_Member_create();
Expression_Call* Expression_Call_create();

void Expression_destroy(Expression* self);

#endif
