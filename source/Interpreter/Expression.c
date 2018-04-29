#include <Interpreter/Expression.h>

Expression_Cast *Expression_Cast_create()
{
    Expression_Cast *self = (Expression_Cast *)malloc(sizeof(Expression_Cast));
    self->expression_id = EXPRESSION_CAST;
    self->type = NULL;
    self->expression = NULL;
    return self;
}

Expression_Size *Expression_Size_create()
{
    Expression_Size *self = (Expression_Size *)malloc(sizeof(Expression_Size));
    self->expression_id = EXPRESSION_SIZE;
    self->type = NULL;
    return self;
}

Expression_Type *Expression_Type_create()
{
    Expression_Type *self = (Expression_Type *)malloc(sizeof(Expression_Type));
    self->expression_id = EXPRESSION_TYPE;
    self->expression = NULL;
    return self;
}

Expression_Assign *Expression_Assign_create()
{
    Expression_Assign *self =
        (Expression_Assign *)malloc(sizeof(Expression_Assign));
    self->expression_id = EXPRESSION_ASSIGN;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

Expression_Binary *Expression_Binary_create()
{
    Expression_Binary *self =
        (Expression_Binary *)malloc(sizeof(Expression_Binary));
    self->expression_id = EXPRESSION_BINARY;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

Expression_Unary *Expression_Unary_create()
{
    Expression_Unary *self =
        (Expression_Unary *)malloc(sizeof(Expression_Unary));
    self->expression_id = EXPRESSION_UNARY;
    self->expression = NULL;
    return self;
}

Expression_Integer_Literal *Expression_Integer_Literal_create()
{
    Expression_Integer_Literal *self = (Expression_Integer_Literal *)malloc(
        sizeof(Expression_Integer_Literal));
    self->expression_id = EXPRESSION_INTEGER_LITERAL;
    return self;
}

Expression_Real_Literal *Expression_Real_Literal_create()
{
    Expression_Real_Literal *self =
        (Expression_Real_Literal *)malloc(sizeof(Expression_Real_Literal));
    self->expression_id = EXPRESSION_REAL_LITERAL;
    return self;
}

Expression_String_Literal *Expression_String_Literal_create()
{
    Expression_String_Literal *self =
        (Expression_String_Literal *)malloc(sizeof(Expression_String_Literal));
    self->expression_id = EXPRESSION_STRING_LITERAL;
    return self;
}

Expression_Tuple *Expression_Tuple_create()
{
    Expression_Tuple *self =
        (Expression_Tuple *)malloc(sizeof(Expression_Tuple));
    self->expression_id = EXPRESSION_TUPLE;
    self->fields = STACK_CREATE(Expression *);
    return self;
}

Expression_Reference *Expression_Reference_create()
{
    Expression_Reference *self =
        (Expression_Reference *)malloc(sizeof(Expression_Reference));
    self->expression_id = EXPRESSION_REFERENCE;
    return self;
}

Expression_Subscripting *Expression_Subscripting_create()
{
    Expression_Subscripting *self =
        (Expression_Subscripting *)malloc(sizeof(Expression_Subscripting));
    self->expression_id = EXPRESSION_SUBSCRIPTING;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

Expression_Member *Expression_Member_create()
{
    Expression_Member *self =
        (Expression_Member *)malloc(sizeof(Expression_Member));
    self->expression_id = EXPRESSION_MEMBER;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

Expression_Call *Expression_Call_create()
{
    Expression_Call *self = (Expression_Call *)malloc(sizeof(Expression_Call));
    self->expression_id = EXPRESSION_MEMBER;
    self->callee = NULL;
    self->arguments = STACK_CREATE(Expression *);
    return self;
}

void Expression_Cast_destroy(Expression_Cast *self)
{
    Type_destroy(self->type);
    Expression_destroy(self->expression);
    free(self);
}

void Expression_Size_destroy(Expression_Size *self)
{
    Type_destroy(self->type);
    free(self);
}

void Expression_Type_destroy(Expression_Type *self)
{
    Expression_destroy(self->expression);
    free(self);
}

void Expression_Assign_destroy(Expression_Assign *self)
{
    Expression_destroy(self->lhs);
    Expression_destroy(self->rhs);
    free(self);
}

void Expression_Binary_destroy(Expression_Binary *self)
{
    Expression_destroy(self->lhs);
    Expression_destroy(self->rhs);
    free(self);
}

void Expression_Unary_destroy(Expression_Unary *self)
{
    Expression_destroy(self->expression);
    free(self);
}

void Expression_Integer_Literal_destroy(Expression_Integer_Literal *self)
{
    free(self);
}

void Expression_Real_Literal_destroy(Expression_Real_Literal *self)
{
    free(self);
}

void Expression_String_Literal_destroy(Expression_String_Literal *self)
{
    free(self);
}

void Expression_Tuple_destroy(Expression_Tuple *self)
{
    for (size_t i = 0; i < self->fields.size; i++)
    {
        Expression_destroy(STACK_GET(Expression *, self->fields, i));
    }

    STACK_DESTROY(Expression *, self->fields);
    free(self);
}

void Expression_Reference_destroy(Expression_Reference *self) { free(self); }

void Expression_Subscripting_destroy(Expression_Subscripting *self)
{
    Expression_destroy(self->lhs);
    Expression_destroy(self->rhs);
    free(self);
}

void Expression_Member_destroy(Expression_Member *self)
{
    Expression_destroy(self->lhs);
    Expression_destroy(self->rhs);
    free(self);
}

void Expression_Call_destroy(Expression_Call *self)
{
    Expression_destroy(self->callee);

    for (size_t i = 0; i < self->arguments.size; i++)
    {
        Expression_destroy(STACK_GET(Expression *, self->arguments, i));
    }

    STACK_DESTROY(Expression *, self->arguments);
    free(self);
}

void Expression_destroy(Expression *self)
{
    switch (self->expression_id)
    {
        case EXPRESSION_SIZE:
        {
            Expression_Size_destroy((Expression_Size *)self);
            break;
        }
        case EXPRESSION_CAST:
        {
            Expression_Cast_destroy((Expression_Cast *)self);
            break;
        }
        case EXPRESSION_TYPE:
        {
            Expression_Type_destroy((Expression_Type *)self);
            break;
        }
        case EXPRESSION_ASSIGN:
        {
            Expression_Assign_destroy((Expression_Assign *)self);
            break;
        }
        case EXPRESSION_BINARY:
        {
            Expression_Binary_destroy((Expression_Binary *)self);
            break;
        }
        case EXPRESSION_UNARY:
        {
            Expression_Unary_destroy((Expression_Unary *)self);
            break;
        }
        case EXPRESSION_INTEGER_LITERAL:
        {
            Expression_Integer_Literal_destroy(
                (Expression_Integer_Literal *)self);
            break;
        }
        case EXPRESSION_REAL_LITERAL:
        {
            Expression_Real_Literal_destroy((Expression_Real_Literal *)self);
            break;
        }
        case EXPRESSION_STRING_LITERAL:
        {
            Expression_String_Literal_destroy(
                (Expression_String_Literal *)self);
            break;
        }
        case EXPRESSION_TUPLE:
        {
            Expression_Tuple_destroy((Expression_Tuple *)self);
            break;
        }
        case EXPRESSION_REFERENCE:
        {
            Expression_Reference_destroy((Expression_Reference *)self);
            break;
        }
        case EXPRESSION_SUBSCRIPTING:
        {
            Expression_Subscripting_destroy((Expression_Subscripting *)self);
            break;
        }
        case EXPRESSION_MEMBER:
        {
            Expression_Member_destroy((Expression_Member *)self);
            break;
        }
        case EXPRESSION_CALL:
        {
            Expression_Call_destroy((Expression_Call *)self);
            break;
        }
    }
}
