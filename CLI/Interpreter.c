#include <CLI/Interpreter.h>
#include <Machine/Execution.h>
#include <Support/String.h>

typedef struct Symbol
{
    void* value;
    Type* type;
    Boolean is_const;
    Boolean has_initialized;
} Symbol;

typedef struct Scope
{
    struct Scope* parent;
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

Expression* parse_expression0(Interpreter* self);
Expression* parse_expression0_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression1(Interpreter* self);
Expression* parse_expression1_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression2(Interpreter* self);
Expression* parse_expression2_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression3(Interpreter* self);
Expression* parse_expression3_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression4(Interpreter* self);
Expression* parse_expression4_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression5(Interpreter* self);
Expression* parse_expression5_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression6(Interpreter* self);
Expression* parse_expression6_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression7(Interpreter* self);
Expression* parse_expression7_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression8(Interpreter* self);
Expression* parse_expression8_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression9(Interpreter* self);
Expression* parse_expression9_tail(Interpreter* self, Expression* lhs);

Expression* parse_expression_prefix(Interpreter* self);
Expression* parse_expression_root(Interpreter* self);

void Interpreter_push_scope(Interpreter* self);
void Interpreter_pop_scope(Interpreter* self);
Symbol* Interpreter_get_symbol(Interpreter* self, String id);
void Interpreter_set_local_symbol(Interpreter* self, String id, Symbol symbol);
void Interpreter_set_global_symbol(Interpreter* self, String id, Symbol symbol);

void Interpreter_next_token(Interpreter* self);
void Interpreter_push_token(Interpreter* self, Token token);

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

void Interpreter_loop()
{
    u8_type_name = String_new("U8", 2);
    u16_type_name = String_new("U16", 3);
    u32_type_name = String_new("U32", 3);
    u64_type_name = String_new("U64", 3);
    s8_type_name = String_new("S8", 2);
    s16_type_name = String_new("S16", 3);
    s32_type_name = String_new("S32", 3);
    s64_type_name = String_new("S64", 3);
    fp32_type_name = String_new("FP32", 4);
    fp64_type_name = String_new("FP64", 4);
    type_type_name = String_new("Type", 4);

    Interpreter self;
    self.machine = Machine_create();
    self.parser.queue = Stack_create(sizeof(Token));

    self.scopes.local = NULL;
    Interpreter_push_scope(&self);
    self.scopes.global = self.scopes.local;

    Interpreter_set_global_symbol(&self, u8_type_name,
                                  (Symbol){type_u8, type_type, True, True});

    Interpreter_set_global_symbol(&self, u16_type_name,
                                  (Symbol){type_u16, type_type, True, True});

    Interpreter_set_global_symbol(&self, u32_type_name,
                                  (Symbol){type_u32, type_type, True, True});

    Interpreter_set_global_symbol(&self, u64_type_name,
                                  (Symbol){type_u64, type_type, True, True});

    Interpreter_set_global_symbol(&self, s8_type_name,
                                  (Symbol){type_s8, type_type, True, True});

    Interpreter_set_global_symbol(&self, s16_type_name,
                                  (Symbol){type_s16, type_type, True, True});

    Interpreter_set_global_symbol(&self, s32_type_name,
                                  (Symbol){type_s32, type_type, True, True});

    Interpreter_set_global_symbol(&self, s64_type_name,
                                  (Symbol){type_s64, type_type, True, True});

    Interpreter_set_global_symbol(&self, fp32_type_name,
                                  (Symbol){type_fp32, type_type, True, True});

    Interpreter_set_global_symbol(&self, fp64_type_name,
                                  (Symbol){type_fp64, type_type, True, True});

    Interpreter_set_global_symbol(&self, type_type_name,
                                  (Symbol){type_type, type_type, True, True});

    Lexer_init();

    for (size_t i = 0;; i++)
    {
        Print("omega:%" PRIu64 "> ", i);

        char* line = Read();

        self.parser.queue_it = 0;
        Stack_clear(&self.parser.queue);

        self.parser.lexer =
            Lexer_create(line, String_fmt("omega:%" PRIu64 "", i));

        Expression* expression = parse_expression0(&self);

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

Expression* parse_expression0(Interpreter* self)
{
    return parse_expression0_tail(self, parse_expression1(self));
}

Expression* parse_expression0_tail(Interpreter* self, Expression* lhs)
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
            Expression* rhs = parse_expression0(self);

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

Expression* parse_expression1(Interpreter* self)
{
    return parse_expression1_tail(self, parse_expression2(self));
}

Expression* parse_expression1_tail(Interpreter* self, Expression* lhs)
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

                Expression* rhs = parse_expression3(self);

                if (!rhs)
                {
                    Expression_destroy(lhs);

                    return NULL;
                }

                Stack_push(&tuple->fields, &rhs);
            }
            else
            {
                Expression* rhs = parse_expression3(self);

                if (!rhs)
                {
                    Expression_destroy(lhs);

                    return NULL;
                }

                tuple = Expression_Tuple_create();
                Stack_push(&tuple->fields, &lhs);
                Stack_push(&tuple->fields, &rhs);
            }

            return parse_expression1_tail(self, (Expression*)tuple);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression2(Interpreter* self)
{
    return parse_expression2_tail(self, parse_expression3(self));
}

Expression* parse_expression2_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression3(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression2_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression3(Interpreter* self)
{
    return parse_expression3_tail(self, parse_expression4(self));
}

Expression* parse_expression3_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression4(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression3_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression4(Interpreter* self)
{
    return parse_expression4_tail(self, parse_expression5(self));
}

Expression* parse_expression4_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression5(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression4_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression5(Interpreter* self)
{
    return parse_expression5_tail(self, parse_expression6(self));
}

Expression* parse_expression5_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression6(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression5_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression6(Interpreter* self)
{
    return parse_expression6_tail(self, parse_expression7(self));
}

Expression* parse_expression6_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression7(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression6_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression7(Interpreter* self)
{
    return parse_expression7_tail(self, parse_expression8(self));
}

Expression* parse_expression7_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression8(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression7_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression8(Interpreter* self)
{
    return parse_expression8_tail(self, parse_expression9(self));
}

Expression* parse_expression8_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression9(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression8_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression9(Interpreter* self)
{
    return parse_expression9_tail(self, parse_expression_prefix(self));
}

Expression* parse_expression9_tail(Interpreter* self, Expression* lhs)
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

            Expression* rhs = parse_expression_prefix(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary* binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return parse_expression9_tail(self, (Expression*)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return lhs;
        }
    }
}

Expression* parse_expression_prefix(Interpreter* self)
{
    Interpreter_next_token(self);

    switch (self->parser.token.tag)
    {
        case '*':
        case '!':
        case '-':
        case '~':
        case '&':
        {
            int op = self->parser.token.tag;

            Expression* expression = parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            if (op == '*' && expression->expression_id == EXPRESSION_TYPE)
            {
                Expression_Type* type_expression =
                    (Expression_Type*)(expression);

                type_expression->type =
                    (Type*)(Type_Pointer_create(type_expression->type));
                return (Expression*)(type_expression);
            }
            else
            {
                Expression_Unary* unary = Expression_Unary_create();
                unary->op = op;
                unary->expression = expression;
                return (Expression*)(unary);
            }
        }
        default:
        {
            Interpreter_push_token(self, self->parser.token);
            return parse_expression_root(self);
        }
    }
}

Expression* parse_expression_root(Interpreter* self)
{
    Interpreter_next_token(self);

    switch (self->parser.token.tag)
    {
        case '(':
        {
            Expression* expression = parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->parser.token.tag != ')')
            {
                Print("Missing closing braces\n");
                return NULL;
            }

            return expression;
        }
        /*
        case Tag_S_CAST:
        {
            Interpreter_next_token(self);

            if (self->parser.token.tag != '(')
            {
                return NULL;
            }

            Type* type = parse_type(self);

            if (!type)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->parser.token.tag != ',')
            {
                return NULL;
            }

            Expression* expression = parse_expression0(self);

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
        */
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
        case '[':
        {
        }
        case Tag_ID:
        {
            Symbol* symbol =
                Interpreter_get_symbol(self, self->parser.token.lexeme);

            if (!symbol)
            {
                Print("Symbol %s is not defined\n",
                      String_begin(self->parser.token.lexeme));
                return NULL;
            }

            if (symbol->type->type_id == TYPE_TYPE)
            {
                Expression_Type* type_expression = Expression_Type_create();
                type_expression->type = (Type*)(symbol->value);
                return (Expression*)type_expression;
            }
            else
            {
                Expression_Reference* reference_expression =
                    Expression_Reference_create();
                reference_expression->id = self->parser.token.lexeme;
                return (Expression*)reference_expression;
            }
        }
        default:
        {
            Print("Expected a valid expression.\n");
            return NULL;
        }
    }
}
