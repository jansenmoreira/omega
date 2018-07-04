#include <Language/AST.h>

AST_Program* AST_Program_create()
{
    AST_Program* self = (AST_Program*)malloc(sizeof(AST_Program));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_PROGRAM;
    self->statements = Stack_create(sizeof(AST_Program));
    self->data = 0;
    return self;
}

AST_Declare* AST_Declare_create()
{
    AST_Declare* self = (AST_Declare*)malloc(sizeof(AST_Declare));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_DECLARE;
    self->ids = Stack_create(sizeof(String));
    self->is_const = False;
    self->type = NULL;
    self->initializer = NULL;
    return self;
}

AST_Function* AST_Function_create()
{
    AST_Function* self = (AST_Function*)malloc(sizeof(AST_Function));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_FUNCTION;
    self->id = String_new(NULL, 0);
    self->in_ids = Stack_create(sizeof(String));
    self->in_types = Stack_create(sizeof(Type*));
    self->out_ids = Stack_create(sizeof(String));
    self->out_types = Stack_create(sizeof(Type*));
    self->body = NULL;
    self->data = 0;
    return self;
}

AST_Block* AST_Block_create()
{
    AST_Block* self = (AST_Block*)malloc(sizeof(AST_Block));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_BLOCK;
    self->statements = Stack_create(sizeof(AST_Block));
    return self;
}

AST_While* AST_While_create()
{
    AST_While* self = (AST_While*)malloc(sizeof(AST_While));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_WHILE;
    self->condition = NULL;
    self->then = NULL;
    return self;
}

AST_If* AST_If_create()
{
    AST_If* self = (AST_If*)malloc(sizeof(AST_If));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_IF;
    self->condition = NULL;
    self->then = NULL;
    self->else_ = NULL;
    return self;
}

AST_Assign* AST_Assign_create()
{
    AST_Assign* self = (AST_Assign*)malloc(sizeof(AST_Assign));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_ASSIGN;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

AST_Tuple* AST_Tuple_create()
{
    AST_Tuple* self = (AST_Tuple*)malloc(sizeof(AST_Tuple));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_TUPLE;
    self->type = NULL;
    self->fields = Stack_create(sizeof(AST*));
    return self;
}

AST_Binary* AST_Binary_create()
{
    AST_Binary* self = (AST_Binary*)malloc(sizeof(AST_Binary));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_BINARY;
    self->type = NULL;
    self->lhs = NULL;
    self->rhs = NULL;
    return self;
}

AST_Unary* AST_Unary_create()
{
    AST_Unary* self = (AST_Unary*)malloc(sizeof(AST_Unary));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_UNARY;
    self->type = NULL;
    self->expression = NULL;
    return self;
}

AST_Integer_Literal* AST_Integer_Literal_create()
{
    AST_Integer_Literal* self =
        (AST_Integer_Literal*)malloc(sizeof(AST_Integer_Literal));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_INTEGER_LITERAL;
    self->type = NULL;
    self->value = String_new(NULL, 0);
    return self;
}

AST_Real_Literal* AST_Real_Literal_create()
{
    AST_Real_Literal* self =
        (AST_Real_Literal*)malloc(sizeof(AST_Real_Literal));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_REAL_LITERAL;
    self->type = NULL;
    self->value = String_new(NULL, 0);
    return self;
}

AST_String_Literal* AST_String_Literal_create()
{
    AST_String_Literal* self =
        (AST_String_Literal*)malloc(sizeof(AST_String_Literal));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_STRING_LITERAL;
    self->type = NULL;
    self->value = String_new(NULL, 0);
    return self;
}

AST_Storage* AST_Storage_create()
{
    AST_Storage* self = (AST_Storage*)malloc(sizeof(AST_Storage));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_STORAGE;
    self->type = NULL;
    self->global = False;
    self->offset = 0;
    return self;
}

AST_Address* AST_Address_create()
{
    AST_Address* self = (AST_Address*)malloc(sizeof(AST_Address));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_ADDRESS;
    self->type = NULL;
    self->base = NULL;
    self->offset = NULL;
    return self;
}

AST_Call* AST_Call_create()
{
    AST_Call* self = (AST_Call*)malloc(sizeof(AST_Call));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->AST_id = AST_CALL;
    self->type = NULL;
    self->callee = NULL;
    self->arguments = NULL;
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
            Type_destroy(ast->type);
            AST_destroy((AST*)(ast->initializer));
            Stack_destroy(&ast->ids);
            break;
        }
        case AST_FUNCTION:
        {
            AST_Function* ast = (AST_Function*)(self);
            AST_destroy(ast->body);

            Stack_destroy(&ast->in_ids);

            for (size_t i = 0; i < ast->in_types.size; i++)
            {
                Type_destroy(*(Type**)(Stack_get(&ast->in_types, i)));
            }

            Stack_destroy(&ast->in_types);

            Stack_destroy(&ast->out_ids);

            for (size_t i = 0; i < ast->out_types.size; i++)
            {
                Type_destroy(*(Type**)(Stack_get(&ast->out_types, i)));
            }

            Stack_destroy(&ast->out_types);
            break;
        }
        case AST_WHILE:
        {
            AST_While* ast = (AST_While*)(self);
            AST_destroy((AST*)ast->condition);
            AST_destroy(ast->then);
            break;
        }
        case AST_IF:
        {
            AST_If* ast = (AST_If*)(self);
            AST_destroy((AST*)ast->condition);
            AST_destroy(ast->then);
            AST_destroy(ast->else_);
            break;
        }
        case AST_ASSIGN:
        {
            AST_Assign* ast = (AST_Assign*)(self);
            AST_destroy((AST*)ast->lhs);
            AST_destroy((AST*)ast->rhs);
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
        case AST_BINARY:
        {
            AST_Binary* ast = (AST_Binary*)(self);
            AST_destroy((AST*)ast->lhs);
            AST_destroy((AST*)ast->rhs);
            break;
        }
        case AST_UNARY:
        {
            AST_Unary* ast = (AST_Unary*)(self);
            AST_destroy((AST*)ast->expression);
            break;
        }
        case AST_ADDRESS:
        {
            AST_Address* ast = (AST_Address*)(self);
            AST_destroy((AST*)ast->base);
            AST_destroy((AST*)ast->offset);
            break;
        }
        case AST_CALL:
        {
            AST_Call* ast = (AST_Call*)(self);
            AST_destroy((AST*)ast->callee);
            AST_destroy((AST*)ast->arguments);
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
            Print("%*s<Program global=\"%llu\">\n", spaces, "", ast->data);

            for (size_t i = 0; i < ast->statements.size; i++)
            {
                AST_print_tree(*(AST**)Stack_get(&ast->statements, i),
                               identation + 1);
            }

            Print("%*s</Program>\n", spaces, "");
            break;
        }
        case AST_DECLARE:
        {
            AST_Declare* ast = (AST_Declare*)(self);

            Print("%*s<Declare Type=\"", spaces, "");
            Type_print(ast->type);
            Print("\">\n");

            for (size_t i = 0; i < ast->ids.size; i++)
            {
                Print("%*s<ID>%s</ID>\n", spaces + width, "",
                      String_begin(*(String*)Stack_get(&ast->ids, i)));
            }

            if (ast->initializer)
            {
                Print("%*s<Initializer>\n", spaces + width, "");
                AST_print_tree((AST*)ast->initializer, identation + 2);
                Print("%*s</Initializer>\n", spaces + width, "");
            }

            Print("%*s<Declare>\n", spaces, "");
            break;
        }
        case AST_FUNCTION:
        {
            AST_Function* ast = (AST_Function*)(self);
            Print("%*s<Function id=\"%s\" local=\"%llu\">\n", spaces, "",
                  String_begin(ast->id), ast->data);

            Print("%*s<Input>\n", spaces + width, "");

            for (size_t i = 0; i < ast->in_ids.size; i++)
            {
                Print("%*s<Param type=\"", spaces + 2 * width, "");
                Type_print(*(Type**)(Stack_get(&ast->in_types, i)));
                Print("\">%s</ID>\n",
                      String_begin(*(String*)Stack_get(&ast->in_ids, i)));
            }

            Print("%*s</Input>\n", spaces + width, "");

            Print("%*s<Output>\n", spaces + width, "");

            for (size_t i = 0; i < ast->out_ids.size; i++)
            {
                Print("%*s<Param type=\"", spaces + 2 * width, "");
                Type_print(*(Type**)(Stack_get(&ast->out_types, i)));
                Print("\">%s</ID>\n",
                      String_begin(*(String*)Stack_get(&ast->out_ids, i)));
            }

            Print("%*s</Output>\n", spaces + width, "");

            if (ast->body)
            {
                AST_print_tree(ast->body, identation + 1);
            }

            Print("%*s</Function>\n", spaces, "");
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
        case AST_WHILE:
        {
            AST_While* ast = (AST_While*)(self);
            Print("%*s<While>\n", spaces, "");
            AST_print_tree((AST*)ast->condition, identation + 1);
            AST_print_tree(ast->then, identation + 1);
            Print("%*s</While>\n", spaces, "");
            break;
        }
        case AST_IF:
        {
            AST_If* ast = (AST_If*)(self);
            Print("%*s<If>\n", spaces, "");
            AST_print_tree((AST*)ast->condition, identation + 1);
            AST_print_tree(ast->then, identation + 1);
            AST_print_tree(ast->else_, identation + 1);
            Print("%*s</If>\n", spaces, "");
            break;
        }
        case AST_ASSIGN:
        {
            AST_Assign* ast = (AST_Assign*)(self);
            Print("%*s<Assign>\n", spaces, "");
            AST_print_tree((AST*)ast->lhs, identation + 1);
            AST_print_tree((AST*)ast->rhs, identation + 1);
            Print("%*s<Assign>\n", spaces, "");
            break;
        }
        case AST_TUPLE:
        {
            AST_Tuple* ast = (AST_Tuple*)(self);
            Print("%*s<Tuple type=\"", spaces, "");
            Type_print(ast->type);
            Print("\">\n");

            for (size_t i = 0; i < ast->fields.size; i++)
            {
                AST_print_tree(*(AST**)Stack_get(&ast->fields, i),
                               identation + 1);
            }

            Print("%*s</Tuple>\n", spaces, "");
            break;
        }
        case AST_BINARY:
        {
            AST_Binary* ast = (AST_Binary*)(self);
            Print("%*s<Binary op=\"%s\" type=\"", spaces, "",
                  Operators[ast->op]);
            Type_print(ast->type);
            Print("\">\n");
            AST_print_tree((AST*)ast->lhs, identation + 1);
            AST_print_tree((AST*)ast->rhs, identation + 1);
            Print("%*s<Binary>\n", spaces, "");
            break;
        }
        case AST_UNARY:
        {
            AST_Unary* ast = (AST_Unary*)(self);
            Print("%*s<Unary op=\"%s\" type=\"", spaces, "",
                  Operators[ast->op]);
            Type_print(ast->type);
            Print("\">\n");
            AST_print_tree((AST*)ast->expression, identation + 1);
            Print("%*s<Unary>\n", spaces, "");
            break;
        }
        case AST_INTEGER_LITERAL:
        {
            AST_Integer_Literal* ast = (AST_Integer_Literal*)(self);
            Print("%*s<IntegerLiteral type=\"", spaces, "");
            Type_print(ast->type);
            Print("\">%s</IntegerLiteral>\n", String_begin(ast->value));
            break;
        }
        case AST_REAL_LITERAL:
        {
            AST_Real_Literal* ast = (AST_Real_Literal*)(self);
            Print("%*s<RealLiteral type=\"", spaces, "");
            Type_print(ast->type);
            Print("\">%s</ RealLiteral>\n", String_begin(ast->value));
            break;
        }
        case AST_STRING_LITERAL:
        {
            AST_String_Literal* ast = (AST_String_Literal*)(self);
            Print("%*s<StringLiteral type=\"", spaces, "");
            Type_print(ast->type);
            Print("\">%s</ StringLiteral>\n", String_begin(ast->value));
            break;
        }
        case AST_ADDRESS:
        {
            AST_Address* ast = (AST_Address*)(self);
            Print("%*s<Address>\n", spaces, "");
            AST_print_tree((AST*)ast->base, identation + 1);
            AST_print_tree((AST*)ast->offset, identation + 1);
            Print("%*s<Address>\n", spaces, "");
            break;
        }
        case AST_STORAGE:
        {
            AST_Storage* ast = (AST_Storage*)(self);

            Print("%*s<%s offset=\"%llu\" type=\"", spaces, "",
                  ast->global ? "Global" : "Local", ast->offset);
            Type_print(ast->type);
            Print("\" />\n");

            break;
        }
        case AST_CALL:
        {
            AST_Call* ast = (AST_Call*)(self);
            Print("%*s<Call>\n", spaces, "");
            Print("%*s<Callee>\n", spaces + width, "");
            AST_print_tree((AST*)ast->callee, identation + 2);
            Print("%*s</Callee>\n", spaces + width, "");
            Print("%*s<Arguments>\n", spaces + width, "");
            AST_print_tree((AST*)ast->arguments, identation + 2);
            Print("%*s</Arguments>\n", spaces + width, "");
            Print("%*s<Call>\n", spaces, "");
            break;
        }
    }
}

void AST_print(AST* self)
{
    AST_print_tree(self, 0);
}
