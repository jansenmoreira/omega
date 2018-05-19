#include <CLI/Interpreter.h>
#include <Machine/Execution.h>
#include <Support/String.h>

typedef struct Interpreter
{
    struct Parser
    {
        Lexer lexer;
        Token token;
        Stack queue;
        size_t queue_it;
    } parser;

    Machine machine;

    Map type_table;
} Interpreter;

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

void Interpreter_next_token(Interpreter* self);
void Interpreter_push_token(Interpreter* self, Token token);

void Interpreter_loop()
{
    Interpreter self;
    self.machine = Machine_create();
    self.parser.queue = Stack_create(1024);
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
        /*
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
