#include <Language/AST.h>

AST_Program* AST_Program_create()
{
    AST_Program* self = (AST_Program*)malloc(sizeof(AST_Program));
    self->AST_id = AST_PROGRAM;
    self->statements = Stack_create(sizeof(AST_Program));
    return self;
}

AST_Block* AST_Block_create()
{
    AST_Block* self = (AST_Block*)malloc(sizeof(AST_Block));
    self->AST_id = AST_BLOCK;
    self->statements = Stack_create(sizeof(AST_Block));
    return self;
}

AST_Import* AST_Import_create()
{
    AST_Import* self = (AST_Import*)malloc(sizeof(AST_Import));
    self->AST_id = AST_IMPORT;
    self->path = String_new(NULL, 0);
    return self;
}

AST_Declare* AST_Declare_create()
{
    AST_Declare* self = (AST_Declare*)malloc(sizeof(AST_Declare));
    self->AST_id = AST_DECLARE;
    self->ids = Stack_create(sizeof(String));
    self->is_const = False;
    self->type = NULL;
    self->initializer = NULL;
    return self;
}

AST_Map* AST_Map_create()
{
    AST_Map* self = (AST_Map*)malloc(sizeof(AST_Map));
    self->AST_id = AST_MAP;
    self->input = NULL;
    self->output = NULL;
    return self;
}

AST_Struct* AST_Struct_create()
{
    AST_Struct* self = (AST_Struct*)malloc(sizeof(AST_Struct));
    self->AST_id = AST_STRUCT;
    self->id = String_new(NULL, 0);
    self->types = Stack_create(sizeof(AST*));
    self->ids = Stack_create(sizeof(String));
    return self;
}

AST_Function* AST_Function_create()
{
    AST_Function* self = (AST_Function*)malloc(sizeof(AST_Function));
    self->AST_id = AST_FUNCTION;
    self->in_ids = Stack_create(sizeof(String));
    self->in_type = NULL;
    self->out_ids = Stack_create(sizeof(String));
    self->out_type = NULL;
    self->body = NULL;
    return self;
}

AST_While* AST_While_create()
{
    AST_While* self = (AST_While*)malloc(sizeof(AST_While));
    self->AST_id = AST_WHILE;
    self->condition = NULL;
    self->then = NULL;
    return self;
}

AST_If* AST_If_create()
{
    AST_If* self = (AST_If*)malloc(sizeof(AST_If));
    self->AST_id = AST_IF;
    self->condition = NULL;
    self->then = NULL;
    self->else_ = NULL;
    return self;
}

AST_Size* AST_Size_create()
{
    AST_Size* self = (AST_Size*)malloc(sizeof(AST_Size));
    self->AST_id = AST_SIZE;
    self->expression = NULL;
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
    self->value = String_new(NULL, 0);
    return self;
}

AST_Real_Literal* AST_Real_Literal_create()
{
    AST_Real_Literal* self =
        (AST_Real_Literal*)malloc(sizeof(AST_Real_Literal));
    self->AST_id = AST_REAL_LITERAL;
    self->value = String_new(NULL, 0);
    return self;
}

AST_String_Literal* AST_String_Literal_create()
{
    AST_String_Literal* self =
        (AST_String_Literal*)malloc(sizeof(AST_String_Literal));
    self->AST_id = AST_STRING_LITERAL;
    self->value = String_new(NULL, 0);
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
    self->id = String_new(NULL, 0);
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
    self->id = String_new(NULL, 0);
    return self;
}

AST_Call* AST_Call_create()
{
    AST_Call* self = (AST_Call*)malloc(sizeof(AST_Call));
    self->AST_id = AST_CALL;
    self->callee = NULL;
    self->argument = NULL;
    return self;
}

void AST_destroy(AST* self)
{
    if (!self)
    {
        return;
    }

    switch (self->AST_id)
    {
        case AST_PROGRAM:
        {
            AST_Program* ast = (AST_Program*)(self);

            for (size_t i = 0; i < ast->statements.size; i++)
            {
                AST_destroy(*(AST**)(Stack_get(&ast->statements, i)));
            }

            Stack_destroy(&ast->statements);
            break;
        }
        case AST_BLOCK:
        {
            AST_Block* ast = (AST_Block*)(self);

            for (size_t i = 0; i < ast->statements.size; i++)
            {
                AST_destroy(*(AST**)(Stack_get(&ast->statements, i)));
            }

            Stack_destroy(&ast->statements);
            break;
        }
        case AST_DECLARE:
        {
            AST_Declare* ast = (AST_Declare*)(self);
            AST_destroy(ast->type);
            AST_destroy(ast->initializer);
            Stack_destroy(&ast->ids);
            break;
        }
        case AST_STRUCT:
        {
            AST_Struct* ast = (AST_Struct*)(self);

            for (size_t i = 0; i < ast->types.size; i++)
            {
                AST_destroy(*(AST**)(Stack_get(&ast->types, i)));
            }

            Stack_destroy(&ast->types);
            Stack_destroy(&ast->ids);
            break;
        }
        case AST_FUNCTION:
        {
            AST_Function* ast = (AST_Function*)(self);
            AST_destroy(ast->body);
            Stack_destroy(&ast->in_ids);
            AST_destroy(ast->in_type);
            Stack_destroy(&ast->out_ids);
            AST_destroy(ast->out_type);
            break;
        }
        case AST_MAP:
        {
            AST_Map* ast = (AST_Map*)(self);
            AST_destroy(ast->input);
            AST_destroy(ast->output);
            break;
        }
        case AST_WHILE:
        {
            AST_While* ast = (AST_While*)(self);
            AST_destroy(ast->condition);
            AST_destroy(ast->then);
            break;
        }
        case AST_IF:
        {
            AST_If* ast = (AST_If*)(self);
            AST_destroy(ast->condition);
            AST_destroy(ast->then);
            AST_destroy(ast->else_);
            break;
        }
        case AST_SIZE:
        {
            AST_Size* ast = (AST_Size*)(self);
            AST_destroy(ast->expression);
            break;
        }
        case AST_ASSIGN:
        {
            AST_Assign* ast = (AST_Assign*)(self);
            AST_destroy(ast->lhs);
            AST_destroy(ast->rhs);
            break;
        }
        case AST_BINARY:
        {
            AST_Binary* ast = (AST_Binary*)(self);
            AST_destroy(ast->lhs);
            AST_destroy(ast->rhs);
            break;
        }
        case AST_UNARY:
        {
            AST_Unary* ast = (AST_Unary*)(self);
            AST_destroy(ast->expression);
            break;
        }
        case AST_TUPLE:
        {
            AST_Tuple* ast = (AST_Tuple*)(self);

            for (size_t i = 0; i < ast->fields.size; i++)
            {
                AST_destroy(*(AST**)Stack_get(&ast->fields, i));
            }

            Stack_destroy(&ast->fields);
            break;
        }
        case AST_SUBSCRIPTING:
        {
            AST_Subscripting* ast = (AST_Subscripting*)(self);
            AST_destroy(ast->lhs);
            AST_destroy(ast->rhs);
            break;
        }
        case AST_MEMBER:
        {
            AST_Member* ast = (AST_Member*)(self);
            AST_destroy(ast->lhs);
            break;
        }
        case AST_CALL:
        {
            AST_Call* ast = (AST_Call*)(self);
            AST_destroy(ast->callee);
            AST_destroy(ast->argument);
            break;
        }
    }

    free(self);
}

void AST_print_tree(AST* self, size_t identation)
{
    if (!self)
    {
        return;
    }

    size_t width = 2;
    size_t spaces = identation * width;

    switch (self->AST_id)
    {
        case AST_PROGRAM:
        {
            AST_Program* ast = (AST_Program*)(self);
            Print("%*s<Program>\n", spaces, "");

            for (size_t i = 0; i < ast->statements.size; i++)
            {
                AST_print_tree(*(AST**)Stack_get(&ast->statements, i),
                               identation + 1);
            }

            Print("%*s</Program>\n", spaces, "");
            break;
        }
        case AST_BLOCK:
        {
            AST_Program* ast = (AST_Program*)(self);
            Print("%*s<Block>\n", spaces, "");

            for (size_t i = 0; i < ast->statements.size; i++)
            {
                AST_print_tree(*(AST**)Stack_get(&ast->statements, i),
                               identation + 1);
            }

            Print("%*s</Block>\n", spaces, "");
            break;
        }
        case AST_IMPORT:
        {
            AST_Import* ast = (AST_Import*)(self);
            Print("%*s<Import \"%s\" />\n", spaces, "",
                  String_begin(ast->path));
            break;
        }
        case AST_DECLARE:
        {
            AST_Declare* ast = (AST_Declare*)(self);
            Print("%*s<Declare>\n", spaces, "");

            Print("%*s<IDs>\n", spaces + width, "");

            for (size_t i = 0; i < ast->ids.size; i++)
            {
                Print("%*s<ID>%s</ID>\n", spaces + 2 * width, "",
                      String_begin(*(String*)Stack_get(&ast->ids, i)));
            }

            Print("%*s</IDs>\n", spaces + width, "");

            if (ast->type)
            {
                Print("%*s<Type>\n", spaces + width, "");
                AST_print_tree(ast->type, identation + 2);
                Print("%*s</Type>\n", spaces + width, "");
            }

            if (ast->initializer)
            {
                Print("%*s<Initializer>\n", spaces + width, "");
                AST_print_tree(ast->initializer, identation + 2);
                Print("%*s</Initializer>\n", spaces + width, "");
            }

            Print("%*s<Declare>\n", spaces, "");
            break;
        }
        case AST_STRUCT:
        {
            AST_Struct* ast = (AST_Struct*)(self);
            break;
        }
        case AST_FUNCTION:
        {
            AST_Function* ast = (AST_Function*)(self);
            Print("%*s<Function>\n", spaces, "");

            Print("%*s<InIDs>\n", spaces + width, "");

            for (size_t i = 0; i < ast->in_ids.size; i++)
            {
                Print("%*s<ID>%s</ID>\n", spaces + 2 * width, "",
                      String_begin(*(String*)Stack_get(&ast->in_ids, i)));
            }

            Print("%*s</InIDs>\n", spaces + width, "");

            Print("%*s<InType>\n", spaces + width, "");

            AST_print_tree(ast->in_type, identation + 2);

            Print("%*s</InType>\n", spaces + width, "");

            Print("%*s<OutIDs>\n", spaces + width, "");

            for (size_t i = 0; i < ast->out_ids.size; i++)
            {
                Print("%*s<ID>%s</ID>\n", spaces + 2 * width, "",
                      String_begin(*(String*)Stack_get(&ast->out_ids, i)));
            }

            Print("%*s</OutIDs>\n", spaces + width, "");

            Print("%*s<OutType>\n", spaces + width, "");

            AST_print_tree(ast->out_type, identation + 2);

            Print("%*s</OutType>\n", spaces + width, "");

            AST_print_tree(ast->body, identation + 1);

            Print("%*s<Function>\n", spaces, "");
            break;
        }
        case AST_WHILE:
        {
            AST_While* ast = (AST_While*)(self);
            Print("%*s<While>\n", spaces, "");
            Print("%*s<Condition>\n", spaces + width, "");
            AST_print_tree(ast->condition, identation + 2);
            Print("%*s</Condition>\n", spaces + width, "");
            Print("%*s<Then>\n", spaces + width, "");
            AST_print_tree(ast->then, identation + 2);
            Print("%*s</Then>\n", spaces + width, "");
            Print("%*s<While>\n", spaces, "");
            break;
        }
        case AST_IF:
        {
            AST_If* ast = (AST_If*)(self);
            Print("%*s<If>\n", spaces, "");
            Print("%*s<Condition>\n", spaces + width, "");
            AST_print_tree(ast->condition, identation + 2);
            Print("%*s</Condition>\n", spaces + width, "");
            Print("%*s<Then>\n", spaces + width, "");
            AST_print_tree(ast->then, identation + 2);
            Print("%*s</Then>\n", spaces + width, "");
            Print("%*s<Else>\n", spaces + width, "");
            AST_print_tree(ast->else_, identation + 2);
            Print("%*s</Else>\n", spaces + width, "");
            Print("%*s</If>\n", spaces, "");
            break;
        }
        case AST_SIZE:
        {
            AST_Size* ast = (AST_Size*)(self);
            break;
        }
        case AST_MAP:
        {
            AST_Map* ast = (AST_Map*)(self);
            Print("%*s<Map>\n", spaces, "");
            Print("%*s<Params>\n", spaces + width, "");
            AST_print_tree(ast->input, identation + 2);
            Print("%*s</Params>\n", spaces + width, "");
            Print("%*s<Result>\n", spaces + width, "");
            AST_print_tree(ast->output, identation + 2);
            Print("%*s</Result>\n", spaces + width, "");
            Print("%*s<Map>\n", spaces, "");
            break;
        }
        case AST_ASSIGN:
        {
            AST_Assign* ast = (AST_Assign*)(self);
            Print("%*s<Assign>\n", spaces, "");
            Print("%*s<LHS>\n", spaces + width, "");
            AST_print_tree(ast->lhs, identation + 2);
            Print("%*s</LHS>\n", spaces + width, "");
            Print("%*s<RHS>\n", spaces + width, "");
            AST_print_tree(ast->rhs, identation + 2);
            Print("%*s</RHS>\n", spaces + width, "");
            Print("%*s<Assign>\n", spaces, "");
            break;
        }
        case AST_BINARY:
        {
            AST_Binary* ast = (AST_Binary*)(self);
            Print("%*s<Binary op=\"%d\">\n", spaces, "", ast->op);
            Print("%*s<LHS>\n", spaces + width, "");
            AST_print_tree(ast->lhs, identation + 2);
            Print("%*s</LHS>\n", spaces + width, "");
            Print("%*s<RHS>\n", spaces + width, "");
            AST_print_tree(ast->rhs, identation + 2);
            Print("%*s</RHS>\n", spaces + width, "");
            Print("%*s<Binary>\n", spaces, "");
            break;
        }
        case AST_UNARY:
        {
            AST_Unary* ast = (AST_Unary*)(self);
            Print("%*s<Unary op=\"%d\">\n", spaces, "", ast->op);
            AST_print_tree(ast->expression, identation + 1);
            Print("%*s<Unary>\n", spaces, "");
            break;
        }
        case AST_INTEGER_LITERAL:
        {
            AST_Integer_Literal* ast = (AST_Integer_Literal*)(self);
            Print("%*s<IntegerLiteral>%s</IntegerLiteral>\n", spaces, "",
                  String_begin(ast->value));
            break;
        }
        case AST_REAL_LITERAL:
        {
            AST_Real_Literal* ast = (AST_Real_Literal*)(self);
            Print("%*s<RealLiteral>%s</RealLiteral>\n", spaces, "",
                  String_begin(ast->value));
            break;
        }
        case AST_STRING_LITERAL:
        {
            AST_String_Literal* ast = (AST_String_Literal*)(self);
            Print("%*s<StringLiteral>%s</StringLiteral>\n", spaces, "",
                  String_begin(ast->value));
            break;
        }
        case AST_TUPLE:
        {
            AST_Tuple* ast = (AST_Tuple*)(self);
            Print("%*s<Tuple>\n", spaces, "");

            for (size_t i = 0; i < ast->fields.size; i++)
            {
                AST_print_tree(*(AST**)Stack_get(&ast->fields, i),
                               identation + 1);
            }

            Print("%*s</Tuple>\n", spaces, "");
            break;
        }
        case AST_REFERENCE:
        {
            AST_Reference* ast = (AST_Reference*)(self);
            Print("%*s<Reference>%s</Reference>\n", spaces, "",
                  String_begin(ast->id));
            break;
        }
        case AST_SUBSCRIPTING:
        {
            AST_Subscripting* ast = (AST_Subscripting*)(self);
            Print("%*s<Subscripting>\n", spaces, "");
            Print("%*s<LHS>\n", spaces + width, "");
            AST_print_tree(ast->lhs, identation + 2);
            Print("%*s</LHS>\n", spaces + width, "");
            Print("%*s<RHS>\n", spaces + width, "");
            AST_print_tree(ast->rhs, identation + 2);
            Print("%*s</RHS>\n", spaces + width, "");
            Print("%*s<Subscripting>\n", spaces, "");
            break;
        }
        case AST_MEMBER:
        {
            AST_Member* ast = (AST_Member*)(self);
            Print("%*s<Member id=\"%s\">\n", spaces, "", String_begin(ast->id));
            AST_print_tree(ast->lhs, identation + 1);
            Print("%*s<Member>\n", spaces, "");
            break;
        }
        case AST_CALL:
        {
            AST_Call* ast = (AST_Call*)(self);
            Print("%*s<Call>\n", spaces, "");
            Print("%*s<Callee>\n", spaces + width, "");
            AST_print_tree(ast->callee, identation + 2);
            Print("%*s</Callee>\n", spaces + width, "");
            Print("%*s<Argument>\n", spaces + width, "");
            AST_print_tree(ast->argument, identation + 2);
            Print("%*s</Argument>\n", spaces + width, "");
            Print("%*s<Call>\n", spaces, "");
            break;
        }
    }
}

void AST_print(AST* self)
{
    AST_print_tree(self, 0);
}

/*
 * Visitor Template
 *

void AST_visitor_template(AST* self)
{
    switch (self->AST_id)
    {
        case AST_PROGRAM:
        {
            AST_Program* ast = (AST_Program*)(self);
            break;
        }
        case AST_IMPORT:
        {
            AST_Import* ast = (AST_Import*)(self);
            break;
        }
        case AST_DECLARE:
        {
            AST_Declare* ast = (AST_Declare*)(self);
            break;
        }
        case AST_STRUCT:
        {
            AST_Struct* ast = (AST_Struct*)(self);
            break;
        }
        case AST_FUNCTION:
        {
            AST_Function* ast = (AST_Function*)(self);
            break;
        }
        case AST_WHILE:
        {
            AST_While* ast = (AST_While*)(self);
            break;
        }
        case AST_IF:
        {
            AST_If* ast = (AST_If*)(self);
            break;
        }
        case AST_SIZE:
        {
            AST_Size* ast = (AST_Size*)(self);
            break;
        }
        case AST_ASSIGN:
        {
            AST_Assign* ast = (AST_Assign*)(self);
            break;
        }
        case AST_BINARY:
        {
            AST_Binary* ast = (AST_Binary*)(self);
            break;
        }
        case AST_UNARY:
        {
            AST_Unary* ast = (AST_Unary*)(self);
            break;
        }
        case AST_INTEGER_LITERAL:
        {
            AST_Integer_Literal* ast = (AST_Integer_Literal*)(self);
            break;
        }
        case AST_REAL_LITERAL:
        {
            AST_Real_Literal* ast = (AST_Real_Literal*)(self);
            break;
        }
        case AST_STRING_LITERAL:
        {
            AST_String_Literal* ast = (AST_String_Literal*)(self);
            break;
        }
        case AST_TUPLE:
        {
            AST_Tuple* ast = (AST_Tuple*)(self);
            break;
        }
        case AST_REFERENCE:
        {
            AST_Reference* ast = (AST_Reference*)(self);
            break;
        }
        case AST_SUBSCRIPTING:
        {
            AST_Subscripting* ast = (AST_Subscripting*)(self);
            break;
        }
        case AST_MEMBER:
        {
            AST_Member* ast = (AST_Member*)(self);
            break;
        }
        case AST_CALL:
        {
            AST_Call* ast = (AST_Call*)(self);
            break;
        }
    }
}
*/
