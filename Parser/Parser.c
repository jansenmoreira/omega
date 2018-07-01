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
AST* Parse_expression_postfix(Parser* self, AST* lhs);

AST* Parse_function(Parser* self);
AST* Parse_statement(Parser* self);
AST* Parse_block(Parser* self);

void Parser_next_token(Parser* self);
void Parser_push_token(Parser* self, Token token);

void Parser_loop()
{
    Parser self;
    self.queue = Stack_create(sizeof(Token));

    Lexer_init();

    for (size_t i = 0;; i++)
    {
        Error("omega:%" PRIu64 "> ", i);

        char* line = Read();

        self.queue_it = 0;
        Stack_clear(&self.queue);

        self.lexer = Lexer_create(line, String_fmt("omega:%" PRIu64 "", i));

        Parser_top_level(&self);

        free(line);
    }

    Lexer_free();
}

AST* Parse_declare(Parser* self)
{
    Tag type_of_declaration = self->token.tag;

    Parser_next_token(self);

    if (self->token.tag != Tag_ID)
    {
        Error("SyntaxError: Expected an identifier\n");
        return NULL;
    }

    String id = self->token.lexeme;

    Parser_next_token(self);

    if (self->token.tag != ':')
    {
        Error("SyntaxError: Expected ':'\n");
        return NULL;
    }

    AST* type = Parse_expression0(self);

    if (!type)
    {
        return NULL;
    }

    Parser_next_token(self);

    AST* initializer = NULL;

    if (self->token.tag == '=')
    {
        initializer = Parse_expression0(self);

        if (!initializer)
        {
            return NULL;
        }

        Parser_next_token(self);
    }

    if (self->token.tag != ';')
    {
        Error("SyntaxError: expected ';'\n");
        return NULL;
    }

    AST_Declare* declaration = AST_Declare_create();
    declaration->id = id;
    declaration->type = type;
    declaration->initializer = initializer;
    return (AST*)(declaration);
}

void Parser_top_level(Parser* self)
{
    AST_Program* ast = AST_Program_create();

    for (Boolean step = True; step;)
    {
        Parser_next_token(self);

        switch (self->token.tag)
        {
            case Tag_END:
            {
                step = False;
                break;
            }
            case Tag_IMPORT:
            {
                Parser_next_token(self);

                if (self->token.tag != Tag_LITERAL_STRING)
                {
                    Error("SyntaxError: Expected an 'String Literal'");
                    return;
                }

                AST_Import* import_ = AST_Import_create();
                import_->path = self->token.lexeme;

                Stack_push(&ast->statements, &import_);
                break;
            }
            case Tag_VAR:
            case Tag_CONST:
            {
                AST* declaration = Parse_declare(self);
                Stack_push(&ast->statements, &declaration);
                break;
            }
            default:
            {
                Parser_push_token(self, self->token);

                AST* expression = Parse_expression0(self);

                Parser_next_token(self);

                if (self->token.tag == '=')
                {
                    AST* rhs = Parse_expression0(self);

                    if (!rhs)
                    {
                        return;
                    }

                    AST_Assign* assign = AST_Assign_create();
                    assign->lhs = expression;
                    assign->rhs = rhs;

                    Stack_push(&ast->statements, &assign);
                }
                else
                {
                    Stack_push(&ast->statements, &expression);
                }

                break;
            }
        }
    }

    AST_print((AST*)(ast));
    AST_destroy((AST*)(ast));
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
        case Tag_ARROW:
        {
            AST* rhs = Parse_expression1(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Map* binary = AST_Map_create();
            binary->params = lhs;
            binary->result = rhs;
            return Parse_expression0_tail(self, (AST*)binary);
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

                AST* rhs = Parse_expression2(self);

                if (!rhs)
                {
                    AST_destroy(lhs);

                    return NULL;
                }

                Stack_push(&tuple->fields, &rhs);
            }
            else
            {
                AST* rhs = Parse_expression2(self);

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

            AST* expression =
                Parse_expression_postfix(self, Parse_expression2(self));

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
            return Parse_expression_postfix(self, Parse_expression_root(self));
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
                Error("SyntaxError: Missing closing parenthesis\n");
                return NULL;
            }

            return expression;
        }
        case Tag_FUNCTION:
        {
            return Parse_function(self);
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
            Error("SyntaxError: Expected a valid expression.\n");
            return NULL;
        }
    }
}

AST* Parse_expression_postfix(Parser* self, AST* lhs)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '[':
        {
            AST* rhs = Parse_expression0(self);

            if (!rhs)
            {
                AST_destroy(lhs);
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != ']')
            {
                Error("SyntaxError: Missing closing brackets\n");
                AST_destroy(lhs);
                return NULL;
            }

            AST_Subscripting* subscripting = AST_Subscripting_create();
            subscripting->lhs = lhs;
            subscripting->rhs = rhs;

            return Parse_expression_postfix(self, (AST*)(subscripting));
        }
        case '(':
        {
            AST* rhs = Parse_expression0(self);

            if (!rhs)
            {
                AST_destroy(lhs);
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                Error("SyntaxError: Missing closing brackets\n");
                AST_destroy(lhs);
                return NULL;
            }

            AST_Call* call = AST_Call_create();
            call->callee = lhs;
            call->argument = rhs;

            return Parse_expression_postfix(self, (AST*)(call));
        }
        case '.':
        {
            Parser_next_token(self);

            if (self->token.tag != Tag_ID)
            {
                Error("SyntaxError: Expected an identifier\n");
                AST_destroy(lhs);
                return NULL;
            }

            String id = self->token.lexeme;

            AST_Member* member = AST_Member_create();
            member->lhs = lhs;
            member->id = id;

            return Parse_expression_postfix(self, (AST*)(member));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST* Parse_statement(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case Tag_IF:
        {
            Parser_next_token(self);

            if (self->token.tag != '(')
            {
                Error("SyntaxError: expected '('");
                return NULL;
            }

            AST_If* if_ = AST_If_create();

            if_->condition = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                Error("SyntaxError: expected ')'");
                return NULL;
            }

            if_->then = Parse_statement(self);

            Parser_next_token(self);

            if (self->token.tag != Tag_ELSE)
            {
                if_->else_ = Parse_statement(self);
            }

            return (AST*)(if_);
        }
        case Tag_WHILE:
        {
            Parser_next_token(self);

            if (self->token.tag != '(')
            {
                Error("SyntaxError: expected '('");
                return NULL;
            }

            AST_While* while_ = AST_While_create();

            while_->condition = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                Error("SyntaxError: expected ')'");
                return NULL;
            }

            while_->then = Parse_statement(self);

            return (AST*)(while_);
        }
        case Tag_CONST:
        case Tag_VAR:
        {
            return Parse_declare(self);
        }
        case '{':
        {
            return (AST*)(Parse_block(self));
        }
        default:
        {
            Parser_push_token(self, self->token);

            AST* expression = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag == '=')
            {
                AST* rhs = Parse_expression0(self);

                if (!rhs)
                {
                    return NULL;
                }

                AST_Assign* assign = AST_Assign_create();
                assign->lhs = expression;
                assign->rhs = rhs;

                expression = (AST*)(assign);
            }

            Parser_next_token(self);

            if (self->token.tag != ';')
            {
                Error("SyntaxError: expected ';'");
                return NULL;
            }

            return expression;
        }
    }
}

AST* Parse_block(Parser* self)
{
    AST_Block* block = AST_Block_create();

    for (;;)
    {
        Parser_next_token(self);

        if (self->token.tag == '}')
        {
            break;
        }
        else
        {
            Parser_push_token(self, self->token);
        }

        AST* statement = Parse_statement(self);
        Stack_push(&block->statements, &statement);
    }

    return (AST*)(block);
}

AST* Parse_function(Parser* self)
{
    Parser_next_token(self);

    if (self->token.tag != '(')
    {
        Error("SyntaxError: expected '('\n");
        return NULL;
    }

    AST_Function* function = AST_Function_create();

    Parser_next_token(self);

    if (self->token.tag != Tag_ARROW)
    {
        Parser_push_token(self, self->token);

        for (Boolean step = True; step;)
        {
            Parser_next_token(self);

            if (self->token.tag != Tag_ID)
            {
                Error("SyntaxError: expected an identifier");
                return NULL;
            }

            Stack_push(&function->in_ids, &self->token.lexeme);

            Parser_next_token(self);

            if (self->token.tag != ',')
            {
                step = False;
            }
        }

        if (self->token.tag != ':')
        {
            Error("SyntaxError: expected ':'");
            return NULL;
        }

        function->in_type = Parse_expression1(self);

        Parser_next_token(self);

        if (self->token.tag != Tag_ARROW)
        {
            Error("SyntaxError: expected '->'");
            return NULL;
        }
    }

    Parser_next_token(self);

    if (self->token.tag != ')')
    {
        Parser_push_token(self, self->token);

        for (Boolean step = True; step;)
        {
            Parser_next_token(self);

            if (self->token.tag != Tag_ID)
            {
                Error("SyntaxError: expected an identifier");
                return NULL;
            }

            Stack_push(&function->out_ids, &self->token.lexeme);

            Parser_next_token(self);

            if (self->token.tag != ',')
            {
                step = False;
            }
        }

        if (self->token.tag != ':')
        {
            Error("SyntaxError: expected ':'");
            return NULL;
        }

        function->out_type = Parse_expression1(self);

        Parser_next_token(self);

        if (self->token.tag != ')')
        {
            Error("SyntaxError: expected ')'\n");
            return NULL;
        }
    }

    Parser_next_token(self);

    if (self->token.tag != '{')
    {
        Error("SyntaxError: expected '{'\n");
        return NULL;
    }

    function->body = (AST*)(Parse_block(self));

    return (AST*)(function);
}
