#include <CLI/Interpreter.h>
#include <Machine/Execution.h>
#include <Support/String.h>

typedef struct Symbol
{
    void* value;
    Type* type;
} Symbol;

typedef struct Scope
{
    Scope* parent;
    Map symbol_table;
} Scope;

typedef struct Interpreter
{
    struct Parser
    {
        Lexer lexer;
        Token token;
        Stack queue;
        size_t queue_it;
    } parser;

    struct Scopes
    {
        Scope* global;
        Scope* local;
    } scopes;

    Machine machine;
} Interpreter;

void Interpreter_push_scope(Interpreter* self)
{
    Scope* parent = self->scopes.local;
    self->scopes.local = (Scope*)malloc(sizeof(Scope));

    if (!self->scopes.local)
    {
        Panic(Memory_Error);
    }

    self->scopes.local->parent = parent;
    self->scopes.local->symbol_table = Map_create(sizeof(Symbol));
}

void Interpreter_pop_scope(Interpreter* self)
{
    assert(self->scopes.local->parent != NULL);

    Scope* scope = self->scopes.local;
    self->scopes.local = self->scopes.local->parent;
    Map_destroy(&scope->symbol_table);
}

Symbol* Interpreter_get_symbol(Interpreter* self, String id)
{
    Scope* scope = self->scopes.local;

    while (scope)
    {
        Symbol* symbol = (Symbol*)Map_get(&scope->symbol_table, id);

        if (symbol)
        {
            return symbol;
        }

        scope = scope->parent;
    }

    return NULL;
}

void Interpreter_set_local_symbol(Interpreter* self, String id, Symbol symbol)
{
    Map_set(&self->scopes.local->symbol_table, id, &symbol);
}

void Interpreter_set_global_symbol(Interpreter* self, String id, Symbol symbol)
{
    Map_set(&self->scopes.global->symbol_table, id, &symbol);
}

void Interpreter_next_token(Interpreter* self);
void Interpreter_push_token(Interpreter* self, Token token);

String u8_type_name;
String u16_type_name;
String u32_type_name;
String u64_type_name;
String s8_type_name;
String s16_type_name;
String s32_type_name;
String s64_type_name;
String fp32_type_name;
String fp64_type_name;
String type_type_name;

Type* u8_type;
Type* u16_type;
Type* u32_type;
Type* u64_type;
Type* s8_type;
Type* s16_type;
Type* s32_type;
Type* s64_type;
Type* fp32_type;
Type* fp64_type;
Type* type_type;

void Interpreter_loop()
{
    u8_type_name = String_new("u8", 2);
    u16_type_name = String_new("u16", 3);
    u32_type_name = String_new("u32", 3);
    u64_type_name = String_new("u64", 3);
    s8_type_name = String_new("s8", 2);
    s16_type_name = String_new("s16", 3);
    s32_type_name = String_new("s32", 3);
    s64_type_name = String_new("s64", 3);
    fp32_type_name = String_new("fp32", 4);
    fp64_type_name = String_new("fp64", 4);
    type_type_name = String_new("type", 4);

    u8_type = Type_Integer_create(1, 0);
    u16_type = Type_Integer_create(2, 0);
    u32_type = Type_Integer_create(4, 0);
    u64_type = Type_Integer_create(8, 0);
    s8_type = Type_Integer_create(1, 1);
    s16_type = Type_Integer_create(2, 1);
    s32_type = Type_Integer_create(4, 1);
    s64_type = Type_Integer_create(8, 1);
    fp32_type = Type_Float_create(4);
    fp64_type = Type_Float_create(8);
    type_type = Type_Type_create();

    Interpreter self;
    self.machine = Machine_create();
    self.parser.queue = Stack_create(sizeof(Token));

    Interpreter_push_scope(&self);
    self.scopes.global = self.scopes.local;

    Interpreter_set_global_symbol(&self, u8_type_name,
                                  (Symbol){u8_type, type_type});

    Interpreter_set_global_symbol(&self, u16_type_name,
                                  (Symbol){u16_type, type_type});

    Interpreter_set_global_symbol(&self, u32_type_name,
                                  (Symbol){u32_type, type_type});

    Interpreter_set_global_symbol(&self, u64_type_name,
                                  (Symbol){u64_type, type_type});

    Interpreter_set_global_symbol(&self, s8_type_name,
                                  (Symbol){s8_type, type_type});

    Interpreter_set_global_symbol(&self, s16_type_name,
                                  (Symbol){s16_type, type_type});

    Interpreter_set_global_symbol(&self, s32_type_name,
                                  (Symbol){s32_type, type_type});

    Interpreter_set_global_symbol(&self, s64_type_name,
                                  (Symbol){s64_type, type_type});

    Interpreter_set_global_symbol(&self, fp32_type_name,
                                  (Symbol){fp32_type, type_type});

    Interpreter_set_global_symbol(&self, fp64_type_name,
                                  (Symbol){fp64_type, type_type});

    Interpreter_set_global_symbol(&self, type_type_name,
                                  (Symbol){type_type, type_type});

    Lexer_init();

    for (size_t i = 0;; i++)
    {
        Print("omega:%" PRIu64 "> ", i);

        char* line = Read();

        self.parser.queue_it = 0;
        Stack_clear(&self.parser.queue);

        self.parser.lexer =
            Lexer_create(line, String_fmt("omega:%" PRIu64 "", i));

        Expression* expression = Interpreter_parse_expression0(&self);

        free(line);
    }

    Lexer_free();
}

void Interpreter_next_token(Interpreter* self)
{
    if (self->parser.queue.size)
    {
        self->parser.token =
            *(Token*)(Stack_get(&self->parser.queue, self->parser.queue_it++));

        if (self->parser.queue_it == self->parser.queue.size)
        {
            self->parser.queue_it = 0;
            Stack_clear(&self->parser.queue);
        }
    }
    else
    {
        self->parser.token = Lexer_next(&self->parser.lexer);
    }
}

void Interpreter_push_token(Interpreter* self, Token token)
{
    Stack_push(&self->parser.queue, &token);
}

/*

Type* Interpreter_parse_type(Interpreter* self);

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

Type* Interpreter_parse_type(Interpreter* self)
{
    Interpreter_next_token(self);
    switch (self->parser.token.tag)
    {
        case Tag_ID:
        {
            Type** type =
                MAP_GET(Type*, self->machine.types, self->parser.token.lexeme);

            if (!type)
            {
                Print("Type Error: Type %s is undefined\n",
                      String_begin(self->parser.token.lexeme));
                return NULL;
            }

            return Type_Copy(*type);
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

            if (self->parser.token.tag != Tag_LITERAL_INTEGER)
            {
                Interpreter_push_token(self, self->parser.token);
                Type_Tuple* tuple = Type_Tuple_create();

                for (int step = 1; step;)
                {
                    Type* type = Interpreter_parse_type(self);

                    if (!type)
                    {
                        Type_destroy((Type*)tuple);
                        return NULL;
                    }

                    Stack_push(&tuple->fields, &type);
                    Interpreter_next_token(self);

                    switch (self->parser.token.tag)
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

            U64 size =
                strtoull(String_begin(self->parser.token.lexeme), NULL, 10);

            Interpreter_next_token(self);

            if (self->parser.token.tag != ']')
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
            if (self->parser.token.tag != Tag_VOID)
            {
                Interpreter_push_token(self, self->parser.token);
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
                    switch (self->parser.token.tag)
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
                if (self->parser.token.tag != Tag_ARROW)
                {
                    Type_destroy((Type*)function);
                    return NULL;
                }
            }
            Interpreter_next_token(self);
            if (self->parser.token.tag != Tag_VOID)
            {
                Interpreter_push_token(self, self->parser.token);
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
                    switch (self->parser.token.tag)
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
                if (self->parser.token.tag != ')')
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

Expression* Interpreter_parse_expression0(Interpreter* self)
{
    return Interpreter_parse_expression0_tail(
        self, Interpreter_parse_expression1(self));
}

Expression* Interpreter_parse_expression0_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->parser.token.tag)
    {
        case '=':
        {
            Expression* rhs = Interpreter_parse_expression0(self);

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
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression1(Interpreter* self)
{
    return Interpreter_parse_expression1_tail(
        self, Interpreter_parse_expression2(self));
}

Expression* Interpreter_parse_expression1_tail(Interpreter* self,
                                               Expression* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Interpreter_next_token(self);

    switch (self->parser.token.tag)
    {
        case ',':
        {
            Expression_Tuple* tuple;

            if (lhs->expression_id == EXPRESSION_TUPLE)
            {
                tuple = (Expression_Tuple*)(lhs);

                Expression* rhs = Interpreter_parse_expression3(self);

                if (!rhs)
                {
                    Expression_destroy(lhs);

                    return NULL;
                }

                Stack_push(&tuple->fields, &rhs);
            }
            else
            {
                Expression* rhs = Interpreter_parse_expression3(self);

                if (!rhs)
                {
                    Expression_destroy(lhs);

                    return NULL;
                }

                tuple = Expression_Tuple_create();
                Stack_push(&tuple->fields, &lhs);
                Stack_push(&tuple->fields, &rhs);
            }

            return Interpreter_parse_expression1_tail(self, (Expression*)tuple);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case '|':
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case '^':
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case '&':
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case Tag_EQ:
        case Tag_NE:
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case '>':
        case '<':
        case Tag_GE:
        case Tag_LE:
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case Tag_RSHIFT:
        case Tag_LSHIFT:
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case '+':
        case '-':
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
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

    switch (self->parser.token.tag)
    {
        case '*':
        case '/':
        case '%':
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* Interpreter_parse_expression_prefix(Interpreter* self)
{
    Interpreter_next_token(self);

    switch (self->parser.token.tag)
    {
        case '!':
        case '-':
        case '~':
        case '*':
        case '&':
        {
            int op = self->parser.token.tag;

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
            Interpreter_push_token(self, self->parser.token);
            return Interpreter_parse_expression_root(self);
        }
    }
}

Expression* Interpreter_parse_expression_root(Interpreter* self)
{
    Interpreter_next_token(self);

    switch (self->parser.token.tag)
    {
        case '(':
        {
            Expression* expression = Interpreter_parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->parser.token.tag != ')')
            {
                printf("Missing closing braces\n");
                return NULL;
            }

            return expression;
        }
        case Tag_S_CAST:
        {
            Interpreter_next_token(self);

            if (self->parser.token.tag != '(')
            {
                return NULL;
            }

            Type* type = Interpreter_parse_type(self);

            if (!type)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->parser.token.tag != ',')
            {
                return NULL;
            }

            Expression* expression = Interpreter_parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->parser.token.tag != ')')
            {
                return NULL;
            }

            Expression_Cast* cast = Expression_Cast_create();
            cast->type = type;
            cast->expression = expression;
            return (Expression*)cast;
        }
        case Tag_LITERAL_STRING:
        {
            Expression_String_Literal* lit = Expression_String_Literal_create();
            lit->value = self->parser.token.lexeme;
            return (Expression*)lit;
        }
        case Tag_LITERAL_INTEGER:
        {
            Expression_Integer_Literal* lit =
                Expression_Integer_Literal_create();
            lit->value = self->parser.token.lexeme;
            return (Expression*)lit;
        }
        case Tag_LITERAL_CHAR:
        {
            Expression_Integer_Literal* lit =
                Expression_Integer_Literal_create();
            lit->value = String_fmt("%" PRIu8 "",
                                    String_begin(self->parser.token.lexeme)[0]);
            return (Expression*)lit;
        }
        case Tag_LITERAL_REAL:
        {
            Expression_Real_Literal* lit = Expression_Real_Literal_create();
            lit->value = self->parser.token.lexeme;
            return (Expression*)lit;
        }
        case Tag_ID:
        {
            Expression_Reference* ref = Expression_Reference_create();
            ref->id = self->parser.token.lexeme;
            return (Expression*)ref;
        }
        default:
        {
            printf("Expected a valid expression.\n");
            return NULL;
        }
    }
}
*/
