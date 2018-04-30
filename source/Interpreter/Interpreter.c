#include <Interpreter/Interpreter.h>

/*
typedef struct Interpreter
{
    Lexer* lexer;
    Token token;

    STACK(Token) queue;
    size_t queue_it;
} Interpreter;

void Interpreter_next_token(Interpreter* self);

Type* Interpreter_parse_type(Interpreter* self);

void Interpreter_global_declaration(Interpreter* self);

int Interpreter_type_declaration(Interpreter* self);

int Interpreter_variable_declaration(Interpreter* self);

Expression* Interpreter_parse_expression0(Interpreter* self);
Expression* Interpreter_parse_expression0_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression1(Interpreter* self);
Expression* Interpreter_parse_expression1_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression2(Interpreter* self);
Expression* Interpreter_parse_expression2_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression3(Interpreter* self);
Expression* Interpreter_parse_expression3_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression4(Interpreter* self);
Expression* Interpreter_parse_expression4_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression5(Interpreter* self);
Expression* Interpreter_parse_expression5_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression6(Interpreter* self);
Expression* Interpreter_parse_expression6_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression7(Interpreter* self);
Expression* Interpreter_parse_expression7_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression8(Interpreter* self);
Expression* Interpreter_parse_expression8_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression9(Interpreter* self);
Expression* Interpreter_parse_expression9_tail(Interpreter* self,
                                               Expression* lhs);

Expression* Interpreter_parse_expression_prefix(Interpreter* self);

Expression* Interpreter_parse_expression_root(Interpreter* self);

void Interpreter_next_token(Interpreter* self)
{
    if (self->queue.size)
    {
        self->token = STACK_GET(Token, self->queue, self->queue_it++);

        if (self->queue_it == self->queue.size)
        {
            self->queue_it = 0;
            STACK_CLEAR(Token, self->queue);
        }
    }
    else
    {
        self->token = Lexer_next(self->lexer);
    }

    // printf("%d\n", self->token.tag);
}

void Interpreter_push_token(Interpreter* self, Token token)
{
    STACK_PUSH(Token, self->queue, token);
}

void Interpreter_loop()
{
    printf("Omega Interpreter for Windows x86-64\n");

    Interpreter interpreter;

    Scope global = Scope_Create(NULL);
    interpreter.global = &global;
    interpreter.actual = &global;
    interpreter.stack = STACK_CREATE(u8);
    interpreter.types = MAP_CREATE(Type*);
    interpreter.queue = STACK_CREATE(Token);
    interpreter.queue_it = 0;

    

    for (int step = 1; step;)
    {
        printf(">>> ");

        char* line = Read();

        Lexer lexer = Lexer_create(line, String_new(NULL, 0));
        interpreter.lexer = &lexer;
        Interpreter_global_declaration(&interpreter);

        STACK_CLEAR(Token, interpreter.queue);
        interpreter.queue_it = 0;
        free(line);
        Lexer_destroy(&lexer);
    }
}

void Interpreter_global_declaration(Interpreter* self)
{
    for (;;)
    {
        Interpreter_next_token(self);

        switch (self->token.tag)
        {
            case Tag_END:
            {
                return;
            }
            case Tag_TYPE:
            {
                if (!Interpreter_type_declaration(self))
                {
                    puts("SyntaxError");
                    return;
                }

                break;
            }
            case Tag_VAR:
            {
                if (!Interpreter_variable_declaration(self))
                {
                    puts("SyntaxError");
                    return;
                }

                break;
            }
            default:
            {
                Interpreter_push_token(self, self->token);
                Expression* exp = Interpreter_parse_expression0(self);

                if (exp && Interpreter_evaluate(self, exp))
                {
                    Type* type;
                    void* value;

                    Interpreter_stack_get(self, &type, &value);

                    if (type && value)
                    {
                        Type_print(type);
                        printf(" (%" PRIu64 ") => ", Type_size(type));
                        Type_print_value(type, value);
                        puts("");

                        Interpreter_stack_destroy_n(self, 1);
                    }
                }
                else
                {
                    puts("Invalid Expression");
                }

                return;
            }
        }
    }
}

int Interpreter_type_declaration(Interpreter* self)
{
    Interpreter_next_token(self);

    if (self->token.tag != Tag_ID)
    {
        return 0;
    }

    String id = self->token.lexeme;

    Interpreter_next_token(self);

    if (self->token.tag == ';')
    {
        Type* n = NULL;
        MAP_SET(Type*, self->types, id, n);
        return 1;
    }

    if (self->token.tag != '{')
    {
        return 0;
    }

    Type_Struct* type = Type_Struct_create();

    for (int step = 1; step;)
    {
        Interpreter_next_token(self);

        if (self->token.tag != Tag_ID)
        {
            Type_destroy((Type*)type);
            return 0;
        }

        String field_id = self->token.lexeme;

        Interpreter_next_token(self);

        if (self->token.tag != ':')
        {
            Type_destroy((Type*)type);
            return 0;
        }

        Type* field_type = Interpreter_parse_type(self);

        if (!type)
        {
            Type_destroy((Type*)type);
            return 0;
        }

        STACK_PUSH(Type*, type->fields, field_type);
        STACK_PUSH(String, type->ids, field_id);

        Interpreter_next_token(self);

        switch (self->token.tag)
        {
            case '}':
            {
                step = 0;
                break;
            }
            case ',':
            {
                break;
            }
            default:
            {
                Type_destroy((Type*)type);
                return 0;
            }
        }
    }

    Interpreter_next_token(self);

    if (self->token.tag != ';')
    {
        Type_destroy((Type*)type);
        return 0;
    }

    MAP_SET(Type*, self->types, id, type);

    Type_print((Type*)type);
    puts("");

    return 1;
}

Type* Interpreter_parse_type(Interpreter* self)
{
    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case Tag_ID:
        {
            Type** type = MAP_GET(Type*, self->types, self->token.lexeme);

            if (!type)
            {
                printf("Type Error: Type %s is undefined\n",
                       String_begin(self->token.lexeme));
                return NULL;
            }

            return (Type*)Type_Alias_create(self->token.lexeme, (Type*)*type);
        }
        case '*':
        {
            Type* type = Interpreter_parse_type(self);

            if (!type)
            {
                return NULL;
            }

            return (Type*)Type_Pointer_create(type);
        }
        case '[':
        {
            Interpreter_next_token(self);

            if (self->token.tag != Tag_LITERAL_INTEGER)
            {
                Interpreter_push_token(self, self->token);

                Type_Tuple* tuple = Type_Tuple_create();

                for (int step = 1; step;)
                {
                    Type* type = Interpreter_parse_type(self);

                    if (!type)
                    {
                        Type_destroy((Type*)tuple);
                        return NULL;
                    }

                    STACK_PUSH(Type*, tuple->fields, type);

                    Interpreter_next_token(self);

                    switch (self->token.tag)
                    {
                        case ']':
                        {
                            step = 0;
                        }
                        case ',':
                        {
                            break;
                        }
                        default:
                        {
                            Type_destroy((Type*)tuple);
                            return NULL;
                        }
                    }
                }

                return (Type*)tuple;
            }

            u64 size = strtoull(String_begin(self->token.lexeme), NULL, 10);

            Interpreter_next_token(self);

            if (self->token.tag != ']')
            {
                return NULL;
            }

            Type* type = Interpreter_parse_type(self);

            if (!type)
            {
                return NULL;
            }

            return (Type*)Type_Array_create(type, size);
        }
        case '(':
        {
            Type_Function* function = Type_Function_create();

            Interpreter_next_token(self);

            if (self->token.tag != Tag_VOID)
            {
                Interpreter_push_token(self, self->token);

                for (int step = 1; step;)
                {
                    Type* type = Interpreter_parse_type(self);

                    if (!type)
                    {
                        Type_destroy((Type*)function);
                        return NULL;
                    }

                    STACK_PUSH(p_Type, function->params, type);

                    Interpreter_next_token(self);

                    switch (self->token.tag)
                    {
                        case Tag_ARROW:
                        {
                            step = 0;
                        }
                        case ',':
                        {
                            break;
                        }
                        default:
                        {
                            Type_destroy((Type*)function);
                            return NULL;
                        }
                    }
                }
            }
            else
            {
                Interpreter_next_token(self);

                if (self->token.tag != Tag_ARROW)
                {
                    Type_destroy((Type*)function);
                    return NULL;
                }
            }

            Interpreter_next_token(self);

            if (self->token.tag != Tag_VOID)
            {
                Interpreter_push_token(self, self->token);

                Type_Tuple* tuple = Type_Tuple_create();

                for (int step = 1; step;)
                {
                    Type* type = Interpreter_parse_type(self);

                    if (!type)
                    {
                        Type_destroy((Type*)tuple);
                        Type_destroy((Type*)function);
                        return NULL;
                    }

                    STACK_PUSH(Type*, tuple->fields, type);

                    Interpreter_next_token(self);

                    switch (self->token.tag)
                    {
                        case ')':
                        {
                            step = 0;
                        }
                        case ',':
                        {
                            break;
                        }
                        default:
                        {
                            Type_destroy((Type*)tuple);
                            Type_destroy((Type*)function);
                            return NULL;
                        }
                    }
                }

                function->return_type = (Type*)tuple;
            }
            else
            {
                Interpreter_next_token(self);

                if (self->token.tag != ')')
                {
                    Type_destroy((Type*)function);
                    return NULL;
                }

                function->return_type = NULL;
            }

            return (Type*)function;
        }
        default:
        {
            return NULL;
        }
    }
}

int Interpreter_variable_declaration(Interpreter* self)
{
    Interpreter_next_token(self);

    if (self->token.tag != Tag_ID)
    {
        printf(
            "Syntax Error: After 'var' and identifier is expected insted "
            "of %s.",
            String_begin(self->token.lexeme));
        return 0;
    }

    String id = self->token.lexeme;

    if (MAP_GET(size_t, self->global->table, id))
    {
        printf("Redefinition Error: Symbol %s is already defined.\n",
               String_begin(id));
        return 0;
    }

    Interpreter_next_token(self);

    if (self->token.tag != ':')
    {
        puts(
            "Syntax Error: In variable declaration, exptected ':' after "
            "identifier");
        return 0;
    }

    Type* type = Interpreter_parse_type(self);

    if (!type)
    {
        puts("Syntax Error: Expected a valid type for variable");
        return 0;
    }

    Interpreter_next_token(self);

    if (self->token.tag != ';')
    {
        puts(
            "Syntax Error: In variable declaration, exptected ':' after "
            "identifier");
        return 0;
    }

    size_t index = self->global->values.size;

    size_t amount = sizeof(Type*) + Type_size(type);
    STACK_GROW(u8, self->global->values, amount);

    Type** ptr = (Type**)(self->global->values.buffer + index);
    *ptr = type;

    MAP_SET(size_t, self->global->table, id, index);

    return 1;
}

Expression* Interpreter_parse_expression0(Interpreter* self)
{
    return Interpreter_parse_expression0_tail(
        self, Interpreter_parse_expression2(self));
}

Expression* Interpreter_parse_expression0_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '=':
        {
            Expression* rhs = Interpreter_parse_expression2(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Assign* assign = Expression_Assign_create();
            assign->lhs = lhs;
            assign->rhs = rhs;
            return (Expression*)assign;
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression2(Interpreter* self)
{
    return Interpreter_parse_expression2_tail(
        self, Interpreter_parse_expression3(self));
}

Expression* Interpreter_parse_expression2_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '|':
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression3(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression2_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression3(Interpreter* self)
{
    return Interpreter_parse_expression3_tail(
        self, Interpreter_parse_expression4(self));
}

Expression* Interpreter_parse_expression3_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '^':
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression4(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression3_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression4(Interpreter* self)
{
    return Interpreter_parse_expression4_tail(
        self, Interpreter_parse_expression5(self));
}

Expression* Interpreter_parse_expression4_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '&':
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression5(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression4_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression5(Interpreter* self)
{
    return Interpreter_parse_expression5_tail(
        self, Interpreter_parse_expression6(self));
}

Expression* Interpreter_parse_expression5_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case Tag_EQ:
        case Tag_NE:
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression6(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression5_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression6(Interpreter* self)
{
    return Interpreter_parse_expression6_tail(
        self, Interpreter_parse_expression7(self));
}

Expression* Interpreter_parse_expression6_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '>':
        case '<':
        case Tag_GE:
        case Tag_LE:
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression7(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression6_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression7(Interpreter* self)
{
    return Interpreter_parse_expression7_tail(
        self, Interpreter_parse_expression8(self));
}

Expression* Interpreter_parse_expression7_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case Tag_RSHIFT:
        case Tag_LSHIFT:
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression8(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression7_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression8(Interpreter* self)
{
    return Interpreter_parse_expression8_tail(
        self, Interpreter_parse_expression9(self));
}

Expression* Interpreter_parse_expression8_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '+':
        case '-':
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression9(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression8_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression9(Interpreter* self)
{
    return Interpreter_parse_expression9_tail(
        self, Interpreter_parse_expression_prefix(self));
}

Expression* Interpreter_parse_expression9_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '*':
        case '/':
        case '%':
        {
            int op = self->token.tag;

            Expression* rhs = Interpreter_parse_expression_prefix(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression9_tail(self,
                                                      (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression_prefix(Interpreter* self)
{
    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '!':
        case '-':
        case '~':
        case '*':
        case '&':
        {
            int op = self->token.tag;

            Expression* expression = Interpreter_parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Expression_Unary* unary = Expression_Unary_create();
            unary->op = op;
            unary->expression = expression;
            return (Expression*)unary;
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return Interpreter_parse_expression_root(self);
        }
    }
}

Expression* Interpreter_parse_expression_root(Interpreter* self)
{
    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '(':
        {
            Expression* expression = Interpreter_parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->token.tag != ')')
            {
                printf("Missing closing braces\n");
                return NULL;
            }

            return expression;
        }
        case Tag_S_CAST:
        {
            Interpreter_next_token(self);

            if (self->token.tag != '(')
            {
                return NULL;
            }

            Type* type = Interpreter_parse_type(self);

            if (!type)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->token.tag != ',')
            {
                return NULL;
            }

            Expression* expression = Interpreter_parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->token.tag != ')')
            {
                return NULL;
            }

            Expression_Cast* cast = Expression_Cast_create();
            cast->type = type;
            cast->expression = expression;
            return (Expression*)cast;
        }
        case Tag_LITERAL_INTEGER:
        {
            Expression_Integer_Literal* lit =
                Expression_Integer_Literal_create();
            lit->value = self->token.lexeme;
            return (Expression*)lit;
        }
        case Tag_LITERAL_REAL:
        {
            Expression_Real_Literal* lit = Expression_Real_Literal_create();
            lit->value = self->token.lexeme;
            return (Expression*)lit;
        }
        case Tag_ID:
        {
            Expression_Reference* ref = Expression_Reference_create();
            ref->id = self->token.lexeme;
            return (Expression*)ref;
        }
        default:
        {
            printf("Expected an valid expression.\n");
            return NULL;
        }
    }
}

*/
