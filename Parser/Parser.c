#include <Parser\Parser.h>

void Parser_top_level(Parser* self);

AST* Parse_expression0(Parser* self);
AST* Parse_expression0_tail(Parser* self, AST* lhs);

AST* Parse_expression1(Parser* self);
AST* Parse_expression1_tail(Parser* self, AST* lhs);

AST* Parse_expression2(Parser* self);
AST* Parse_expression2_tail(Parser* self, AST* lhs);

AST* Parse_expression3(Parser* self);
AST* Parse_expression3_tail(Parser* self, AST* lhs);

AST* Parse_expression4(Parser* self);
AST* Parse_expression4_tail(Parser* self, AST* lhs);

AST* Parse_expression5(Parser* self);
AST* Parse_expression5_tail(Parser* self, AST* lhs);

AST* Parse_expression6(Parser* self);
AST* Parse_expression6_tail(Parser* self, AST* lhs);

AST* Parse_expression7(Parser* self);
AST* Parse_expression7_tail(Parser* self, AST* lhs);

AST* Parse_expression8(Parser* self);
AST* Parse_expression8_tail(Parser* self, AST* lhs);

AST* Parse_expression9(Parser* self);
AST* Parse_expression9_tail(Parser* self, AST* lhs);

AST* Parse_expression_prefix(Parser* self);
AST* Parse_expression_root(Parser* self);

void Parser_next_token(Parser* self);
void Parser_push_token(Parser* self, Token token);

void Parser_top_level(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case Tag_VAR:
        case Tag_CONST:
        {
            Parser_next_token(self);

            if (self->token.tag != Tag_ID)
            {
                Error("SyntaxError: Expected an identifier\n");
                return;
            }

            // String id = self->token.lexeme;

            Parser_next_token(self);

            if (self->token.tag != ':')
            {
                Error("SyntaxError: Expected ':'\n");
                return;
            }

            AST* type_expression = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag != ';')
            {
                Error("SyntaxError: expected ';'\n");
                return;
            }

            break;
        }
        default:
        {
            break;
        }
    }
}

void Parser_next_token(Parser* self)
{
    if (self->queue.size)
    {
        self->token = *(Token*)(Stack_get(&self->queue, self->queue_it++));

        if (self->queue_it == self->queue.size)
        {
            self->queue_it = 0;
            Stack_clear(&self->queue);
        }
    }
    else
    {
        self->token = Lexer_next(&self->lexer);
    }
}

void Parser_push_token(Parser* self, Token token)
{
    Stack_push(&self->queue, &token);
}

AST* Parse_expression0(Parser* self)
{
    return Parse_expression0_tail(self, Parse_expression1(self));
}

AST* Parse_expression0_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '=':
        {
            AST* rhs = Parse_expression0(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Assign* assign = AST_Assign_create();
            assign->lhs = lhs;
            assign->rhs = rhs;
            return (AST*)assign;
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression1(Parser* self)
{
    return Parse_expression1_tail(self, Parse_expression2(self));
}

AST* Parse_expression1_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case ',':
        {
            AST_Tuple* tuple;

            if (lhs->AST_id == AST_TUPLE)
            {
                tuple = (AST_Tuple*)(lhs);

                AST* rhs = Parse_expression3(self);

                if (!rhs)
                {
                    AST_destroy(lhs);

                    return NULL;
                }

                Stack_push(&tuple->fields, &rhs);
            }
            else
            {
                AST* rhs = Parse_expression3(self);

                if (!rhs)
                {
                    AST_destroy(lhs);

                    return NULL;
                }

                tuple = AST_Tuple_create();
                Stack_push(&tuple->fields, &lhs);
                Stack_push(&tuple->fields, &rhs);
            }

            return Parse_expression1_tail(self, (AST*)tuple);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression2(Parser* self)
{
    return Parse_expression2_tail(self, Parse_expression3(self));
}

AST* Parse_expression2_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '|':
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression3(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression2_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression3(Parser* self)
{
    return Parse_expression3_tail(self, Parse_expression4(self));
}

AST* Parse_expression3_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '^':
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression4(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression3_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression4(Parser* self)
{
    return Parse_expression4_tail(self, Parse_expression5(self));
}

AST* Parse_expression4_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '&':
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression5(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression4_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression5(Parser* self)
{
    return Parse_expression5_tail(self, Parse_expression6(self));
}

AST* Parse_expression5_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case Tag_EQ:
        case Tag_NE:
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression6(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression5_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression6(Parser* self)
{
    return Parse_expression6_tail(self, Parse_expression7(self));
}

AST* Parse_expression6_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '>':
        case '<':
        case Tag_GE:
        case Tag_LE:
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression7(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression6_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression7(Parser* self)
{
    return Parse_expression7_tail(self, Parse_expression8(self));
}

AST* Parse_expression7_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case Tag_RSHIFT:
        case Tag_LSHIFT:
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression8(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression7_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression8(Parser* self)
{
    return Parse_expression8_tail(self, Parse_expression9(self));
}

AST* Parse_expression8_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '+':
        case '-':
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression9(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression8_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression9(Parser* self)
{
    return Parse_expression9_tail(self, Parse_expression_prefix(self));
}

AST* Parse_expression9_tail(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '*':
        case '/':
        case '%':
        {
            int op = self->token.tag;

            AST* rhs = Parse_expression_prefix(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression9_tail(self, (AST*)binary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_expression_prefix(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '*':
        case '&':
        case '!':
        case '-':
        case '~':
        {
            int op = self->token.tag;

            AST* expression = Parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            AST_Unary* unary = AST_Unary_create();
            unary->op = op;
            unary->expression = expression;
            return (AST*)(unary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return Parse_expression_root(self);
        }
    }
}

AST* Parse_expression_root(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '(':
        {
            AST* expression = Parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                Print("SyntaxError: Missing closing braces\n");
                return NULL;
            }

            return expression;
        }
        case Tag_LITERAL_STRING:
        {
            AST_String_Literal* lit = AST_String_Literal_create();
            lit->value = self->token.lexeme;
            return (AST*)lit;
        }
        case Tag_LITERAL_INTEGER:
        {
            AST_Integer_Literal* lit = AST_Integer_Literal_create();
            lit->value = self->token.lexeme;
            return (AST*)lit;
        }
        case Tag_LITERAL_CHAR:
        {
            AST_Integer_Literal* lit = AST_Integer_Literal_create();
            lit->value =
                String_fmt("%" PRIu8 "", String_begin(self->token.lexeme)[0]);
            return (AST*)lit;
        }
        case Tag_LITERAL_REAL:
        {
            AST_Real_Literal* lit = AST_Real_Literal_create();
            lit->value = self->token.lexeme;
            return (AST*)lit;
        }
        case Tag_ID:
        {
            AST_Reference* reference_expression = AST_Reference_create();
            reference_expression->id = self->token.lexeme;
            return (AST*)reference_expression;
        }
        default:
        {
            Print("SyntaxError: Expected a valid expression.\n");
            return NULL;
        }
    }
}
