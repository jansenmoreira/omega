#include <Parser/AST.h>

AST_Program* AST_Program_create()
{
    AST_Program* self = (AST_Program*)malloc(sizeof(AST_Program));
    self->AST_id = AST_PROGRAM;
    self->statements = Stack_create(sizeof(AST_Program));
    return self;
}

AST_Import* AST_Import_create(String path)
{
    AST_Import* self = (AST_Import*)malloc(sizeof(AST_Import));
    self->AST_id = AST_PROGRAM;
    self->path = String_new(NULL, 0);
    return self;
}

AST_Declare* AST_Declare_create()
{
    AST_Declare* self = (AST_Declare*)malloc(sizeof(AST_Declare));
    self->AST_id = AST_PROGRAM;
    self->id = String_new(NULL, 0);
    self->is_const = False;
    self->type = NULL;
    self->initiliazer = NULL;
    return self;
}

AST_Struct* AST_Struct_create()
{
    AST_Struct* self = (AST_Struct*)malloc(sizeof(AST_Struct));
    self->AST_id = AST_STRUCT;
    self->id = String_new(NULL, 0);
    self->types = Stack_create(sizeof(AST*));
    self->ids = Stack_create(sizeof(String));
}

AST_While* AST_While_create()
{
}

AST_If* AST_If_create()
{
}

AST_Size* AST_Size_create()
{
    AST_Size* self = (AST_Size*)malloc(sizeof(AST_Size));
    self->AST_id = AST_SIZE;
    return self;
}

AST_Assign* AST_Assign_create()
{
    AST_Assign* self = (AST_Assign*)malloc(sizeof(AST_Assign));
    self->AST_id = AST_ASSIGN;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

AST_Binary* AST_Binary_create()
{
    AST_Binary* self = (AST_Binary*)malloc(sizeof(AST_Binary));
    self->AST_id = AST_BINARY;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

AST_Unary* AST_Unary_create()
{
    AST_Unary* self = (AST_Unary*)malloc(sizeof(AST_Unary));
    self->AST_id = AST_UNARY;
    self->expression = NULL;
    return self;
}

AST_Integer_Literal* AST_Integer_Literal_create()
{
    AST_Integer_Literal* self =
        (AST_Integer_Literal*)malloc(sizeof(AST_Integer_Literal));
    self->AST_id = AST_INTEGER_LITERAL;
    return self;
}

AST_Real_Literal* AST_Real_Literal_create()
{
    AST_Real_Literal* self =
        (AST_Real_Literal*)malloc(sizeof(AST_Real_Literal));
    self->AST_id = AST_REAL_LITERAL;
    return self;
}

AST_String_Literal* AST_String_Literal_create()
{
    AST_String_Literal* self =
        (AST_String_Literal*)malloc(sizeof(AST_String_Literal));
    self->AST_id = AST_STRING_LITERAL;
    return self;
}

AST_Tuple* AST_Tuple_create()
{
    AST_Tuple* self = (AST_Tuple*)malloc(sizeof(AST_Tuple));
    self->AST_id = AST_TUPLE;
    self->fields = Stack_create(sizeof(AST*));
    return self;
}

AST_Reference* AST_Reference_create()
{
    AST_Reference* self = (AST_Reference*)malloc(sizeof(AST_Reference));
    self->AST_id = AST_REFERENCE;
    return self;
}

AST_Subscripting* AST_Subscripting_create()
{
    AST_Subscripting* self =
        (AST_Subscripting*)malloc(sizeof(AST_Subscripting));
    self->AST_id = AST_SUBSCRIPTING;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

AST_Member* AST_Member_create()
{
    AST_Member* self = (AST_Member*)malloc(sizeof(AST_Member));
    self->AST_id = AST_MEMBER;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

AST_Call* AST_Call_create()
{
    AST_Call* self = (AST_Call*)malloc(sizeof(AST_Call));
    self->AST_id = AST_MEMBER;
    self->callee = NULL;
    self->arguments = Stack_create(sizeof(AST*));
    return self;
}

void AST_Size_destroy(AST_Size* self)
{
    free(self);
}

void AST_Assign_destroy(AST_Assign* self)
{
    AST_destroy(self->lhs);
    AST_destroy(self->rhs);
    free(self);
}

void AST_Binary_destroy(AST_Binary* self)
{
    AST_destroy(self->lhs);
    AST_destroy(self->rhs);
    free(self);
}

void AST_Unary_destroy(AST_Unary* self)
{
    AST_destroy(self->expression);
    free(self);
}

void AST_Integer_Literal_destroy(AST_Integer_Literal* self)
{
    free(self);
}

void AST_Real_Literal_destroy(AST_Real_Literal* self)
{
    free(self);
}

void AST_String_Literal_destroy(AST_String_Literal* self)
{
    free(self);
}

void AST_Tuple_destroy(AST_Tuple* self)
{
    for (size_t i = 0; i < self->fields.size; i++)
    {
        AST_destroy(*(AST**)Stack_get(&self->fields, i));
    }

    Stack_destroy(&self->fields);
    free(self);
}

void AST_Reference_destroy(AST_Reference* self)
{
    free(self);
}

void AST_Subscripting_destroy(AST_Subscripting* self)
{
    AST_destroy(self->lhs);
    AST_destroy(self->rhs);
    free(self);
}

void AST_Member_destroy(AST_Member* self)
{
    AST_destroy(self->lhs);
    AST_destroy(self->rhs);
    free(self);
}

void AST_Call_destroy(AST_Call* self)
{
    AST_destroy(self->callee);

    for (size_t i = 0; i < self->arguments.size; i++)
    {
        AST_destroy(*(AST**)Stack_get(&self->arguments, i));
    }

    Stack_destroy(&self->arguments);
    free(self);
}

void AST_destroy(AST* self)
{
    switch (self->AST_id)
    {
        case AST_SIZE:
        {
            AST_Size_destroy((AST_Size*)self);
            break;
        }
        case AST_ASSIGN:
        {
            AST_Assign_destroy((AST_Assign*)self);
            break;
        }
        case AST_BINARY:
        {
            AST_Binary_destroy((AST_Binary*)self);
            break;
        }
        case AST_UNARY:
        {
            AST_Unary_destroy((AST_Unary*)self);
            break;
        }
        case AST_INTEGER_LITERAL:
        {
            AST_Integer_Literal_destroy((AST_Integer_Literal*)self);
            break;
        }
        case AST_REAL_LITERAL:
        {
            AST_Real_Literal_destroy((AST_Real_Literal*)self);
            break;
        }
        case AST_STRING_LITERAL:
        {
            AST_String_Literal_destroy((AST_String_Literal*)self);
            break;
        }
        case AST_TUPLE:
        {
            AST_Tuple_destroy((AST_Tuple*)self);
            break;
        }
        case AST_REFERENCE:
        {
            AST_Reference_destroy((AST_Reference*)self);
            break;
        }
        case AST_SUBSCRIPTING:
        {
            AST_Subscripting_destroy((AST_Subscripting*)self);
            break;
        }
        case AST_MEMBER:
        {
            AST_Member_destroy((AST_Member*)self);
            break;
        }
        case AST_CALL:
        {
            AST_Call_destroy((AST_Call*)self);
            break;
        }
    }
}
