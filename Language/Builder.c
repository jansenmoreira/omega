/*
#include <Language/Builder.h>
#include <Machine/Execution.h>

Builder Builder_create()
{
}

void Builder_destroy(Builder* self)
{
}

size_t Build_Visitor(AST* self, Builder* builder)
{
    switch (self->AST_id)
    {
        case AST_PROGRAM:
        {
        }
        case AST_BLOCK:
        {
        }
        case AST_IMPORT:
        {
        }
        case AST_DECLARE:
        {
        }
        case AST_STRUCT:
        {
        }
        case AST_FUNCTION:
        {
        }
        case AST_WHILE:
        {
        }
        case AST_IF:
        {
        }
        case AST_SIZE:
        {
        }
        case AST_MAP:
        {
        }
        case AST_ASSIGN:
        {
        }
        case AST_BINARY:
        {
        }
        case AST_UNARY:
        {
        }
        case AST_INTEGER_LITERAL:
        {
        }
        case AST_REAL_LITERAL:
        {
        }
        case AST_STRING_LITERAL:
        {
        }
        case AST_TUPLE:
        {
        }
        case AST_REFERENCE:
        {
        }
        case AST_SUBSCRIPTING:
        {
        }
        case AST_MEMBER:
        {
        }
        case AST_CALL:
        {
        }
    }

    return 0;
}

Program* Builder_build(Builder* self, AST* ast)
{
    Builder builder;
    builder.stack = Stack_create(sizeof(Program));

    if (Build_Visitor(ast, &builder))
    {
        Stack_destroy(&builder.scopes);
        return NULL;
    }
    else
    {
        Program* code = (Program*)malloc(stack.size * stack.sizeof_value);
        memcpy(code, stack.buffer, stack.size * stack.sizeof_value);
        Stack_destroy(&stack);
        return code;
    }
}
*/
