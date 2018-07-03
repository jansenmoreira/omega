#include <Language/Parser.h>

AST* Parse_top_level(Parser* self);

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

static void LogError(Token token, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    Position position = File_position(token.file, token.begin, True);

    Error("SyntaxError (%s:%" PRIu64 ":% " PRIu64 "): ",
          String_begin(token.file->path), position.line, position.column);

    Error_va(format, args);

    Error("\n");

    va_end(args);
}

AST* Parse(String path)
{
    Parser self;
    self.queue = Stack_create(sizeof(Token));
    self.queue_it = 0;

    File* file = File_open(path);

    if (!file)
    {
        Error("I/O.Error: \"%s\" not found!\n", String_begin(path));
        return NULL;
    }

    self.lexer = Lexer_create(file);

    AST_Program* program = AST_Program_create();

    for (;;)
    {
        Parser_next_token(&self);

        if (self.token.tag == Tag_END)
        {
            break;
        }
        else
        {
            Parser_push_token(&self, self.token);
        }

        AST* statement = Parse_top_level(&self);

        if (!statement)
        {
            while (!(self.token.tag == ';' || self.token.tag == Tag_END))
            {
                Parser_next_token(&self);
            }

            if (self.token.tag == Tag_END)
            {
                Parser_push_token(&self, self.token);
            }

            continue;
        }

        if (statement->AST_id == AST_PROGRAM)
        {
            AST_Program* ast = (AST_Program*)(statement);
            Stack_push_stack(&program->statements, &ast->statements);
            Stack_destroy(&ast->statements);
        }
        else
        {
            Stack_push(&program->statements, &statement);
        }
    }

    return (AST*)(program);
}

AST* Parse_declare(Parser* self)
{
    AST_Declare* declaration = AST_Declare_create();

    declaration->is_const = self->token.tag == Tag_CONST;

    for (Boolean step = True; step;)
    {
        Parser_next_token(self);

        if (self->token.tag != Tag_ID)
        {
            LogError(self->token, "expected an identifier");
            return NULL;
        }

        Stack_push(&declaration->ids, &self->token.lexeme);

        Parser_next_token(self);

        if (self->token.tag != ',')
        {
            step = False;
        }
    }

    if (self->token.tag != ':')
    {
        LogError(self->token, "Expected ':'");
        return NULL;
    }

    Parser_next_token(self);

    if (self->token.tag == '=')
    {
        declaration->initializer = Parse_expression0(self);

        if (!declaration->initializer)
        {
            AST_destroy((AST*)(declaration));
            return NULL;
        }

        Parser_next_token(self);
    }
    else
    {
        Parser_push_token(self, self->token);

        declaration->type = Parse_expression0(self);

        if (!declaration->type)
        {
            AST_destroy((AST*)(declaration));
            return NULL;
        }

        Parser_next_token(self);

        if (self->token.tag == '=')
        {
            declaration->initializer = Parse_expression0(self);

            if (!declaration->initializer)
            {
                AST_destroy((AST*)(declaration));
                return NULL;
            }

            Parser_next_token(self);
        }
    }

    if (self->token.tag != ';')
    {
        LogError(self->token, "Expected ';'");
        AST_destroy((AST*)(declaration));
        return NULL;
    }

    return (AST*)(declaration);
}

AST* Parse_top_level(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case Tag_IMPORT:
        {
            Parser_next_token(self);

            if (self->token.tag != Tag_LITERAL_STRING)
            {
                LogError(self->token, "Expected a 'String Literal'");
                return NULL;
            }

            String path = self->token.lexeme;

            Parser_next_token(self);

            if (self->token.tag != ';')
            {
                LogError(self->token, "Expected ';'");
                return NULL;
            }

            if (File_exists(path))
            {
                return (AST*)(AST_Program_create());
            }

            return Parse(path);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return Parse_statement(self);
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
        case Tag_ARROW:
        {
            AST* rhs = Parse_expression1(self);

            if (!rhs)
            {
                return NULL;
            }

            AST_Map* map = AST_Map_create();
            map->input = lhs;
            map->output = rhs;
            return Parse_expression0_tail(self, (AST*)(map));
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
                LogError(self->token, "Expected ')'");
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
        case Tag_ARROW:
        {
            return (AST*)(AST_Map_create());
        }
        default:
        {
            LogError(self->token, "Invalid expression");
            return NULL;
        }
    }
}

AST* Parse_expression_postfix(Parser* self, AST* lhs)
{
    if (!lhs)
    {
        return NULL;
    }

    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '[':
        {
            AST_Subscripting* subscripting = AST_Subscripting_create();
            subscripting->lhs = lhs;
            subscripting->rhs = Parse_expression0(self);

            if (!subscripting->rhs)
            {
                AST_destroy((AST*)(subscripting));
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != ']')
            {
                LogError(self->token, "Expected ']'");
                AST_destroy((AST*)(subscripting));
                return NULL;
            }

            return Parse_expression_postfix(self, (AST*)(subscripting));
        }
        case '(':
        {
            AST_Call* call = AST_Call_create();
            call->callee = lhs;

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                Parser_push_token(self, self->token);

                call->argument = Parse_expression0(self);

                if (!call->argument)
                {
                    AST_destroy((AST*)(call));
                    return NULL;
                }

                Parser_next_token(self);

                if (self->token.tag != ')')
                {
                    LogError(self->token, "Expected ')'");
                    AST_destroy((AST*)(call));
                    return NULL;
                }
            }

            return Parse_expression_postfix(self, (AST*)(call));
        }
        case '.':
        {
            Parser_next_token(self);

            if (self->token.tag != Tag_ID)
            {
                LogError(self->token, "Expected an identifier");
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
                LogError(self->token, "Expected '('");
                return NULL;
            }

            AST_If* if_ = AST_If_create();

            if_->condition = Parse_expression0(self);

            if (!if_->condition)
            {
                AST_destroy((AST*)(if_));
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                LogError(self->token, "Expected ')'");
                AST_destroy((AST*)(if_));
                return NULL;
            }

            if_->then = Parse_statement(self);

            if (!if_->then)
            {
                AST_destroy((AST*)(if_));
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag == Tag_ELSE)
            {
                if_->else_ = Parse_statement(self);

                if (!if_->else_)
                {
                    AST_destroy((AST*)(if_));
                    return NULL;
                }
            }
            else
            {
                Parser_push_token(self, self->token);
            }

            return (AST*)(if_);
        }
        case Tag_WHILE:
        {
            Parser_next_token(self);

            if (self->token.tag != '(')
            {
                LogError(self->token, "Expected '('");
                return NULL;
            }

            AST_While* while_ = AST_While_create();

            while_->condition = Parse_expression0(self);

            if (!while_->condition)
            {
                AST_destroy((AST*)(while_));
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                LogError(self->token, "Expected ')'");
                AST_destroy((AST*)(while_));
                return NULL;
            }

            while_->then = Parse_statement(self);

            if (!while_->then)
            {
                AST_destroy((AST*)(while_));
                return NULL;
            }

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

            if (!expression)
            {
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag == '=')
            {
                AST* rhs = Parse_expression0(self);

                if (!rhs)
                {
                    AST_destroy(expression);
                    return NULL;
                }

                AST_Assign* assign = AST_Assign_create();
                assign->lhs = expression;
                assign->rhs = rhs;
                expression = (AST*)(assign);

                Parser_next_token(self);
            }

            if (self->token.tag != ';')
            {
                LogError(self->token, "Expected ';'");
                AST_destroy(expression);
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

        if (!statement)
        {
            while (!(self->token.tag == ';' || self->token.tag == '}'))
            {
                Parser_next_token(self);
            }

            if (self->token.tag == '}')
            {
                Parser_push_token(self, self->token);
            }

            continue;
        }

        Stack_push(&block->statements, &statement);
    }

    return (AST*)(block);
}

AST* Parse_function(Parser* self)
{
    Parser_next_token(self);

    if (self->token.tag != '(')
    {
        LogError(self->token, "Expected '('");
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
                LogError(self->token, "Expected an identifier");
                AST_destroy((AST*)(function));
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
            LogError(self->token, "Expected ':'");
            AST_destroy((AST*)(function));
            return NULL;
        }

        function->in_type = Parse_expression1(self);

        if (!function->in_type)
        {
            AST_destroy((AST*)(function));
            return NULL;
        }

        Parser_next_token(self);

        if (self->token.tag != Tag_ARROW)
        {
            LogError(self->token, "Expected '->'");
            AST_destroy((AST*)(function));
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
                LogError(self->token, "Expected an identifier");
                AST_destroy((AST*)(function));
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
            LogError(self->token, "Expected ':'");
            AST_destroy((AST*)(function));
            return NULL;
        }

        function->out_type = Parse_expression1(self);

        if (!function->out_type)
        {
            AST_destroy((AST*)(function));
            return NULL;
        }

        Parser_next_token(self);

        if (self->token.tag != ')')
        {
            LogError(self->token, "Expected ')'");
            AST_destroy((AST*)(function));
            return NULL;
        }
    }

    Parser_next_token(self);

    if (self->token.tag != '{')
    {
        LogError(self->token, "Expected '{'");
        AST_destroy((AST*)(function));
        return NULL;
    }

    function->body = (AST*)(Parse_block(self));

    if (!function->body)
    {
        AST_destroy((AST*)(function));
        return NULL;
    }

    return (AST*)(function);
}
