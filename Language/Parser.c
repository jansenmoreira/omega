#include <Language/Parser.h>

// @static

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

static String u8_type_name;
static String u16_type_name;
static String u32_type_name;
static String u64_type_name;
static String s8_type_name;
static String s16_type_name;
static String s32_type_name;
static String s64_type_name;
static String fp32_type_name;
static String fp64_type_name;

typedef enum Symbol_Type
{
    SYMBOL_LOCAL_VAR,
    SYMBOL_GLOBAL_VAR,
    SYMBOL_FUNCTION,
    SYMBOL_TYPE
} Symbol_Type;

typedef struct Symbol
{
    size_t offset;
    Type* type;
    Boolean constant;
    Boolean initialized;
    Symbol_Type symbol_type;
} Symbol;

typedef struct Scope
{
    struct Scope* parent;
    Map symbols;
    size_t offset;
} Scope;

static struct Program
{
    Map global;
    size_t global_offset;

    Scope* local;
    size_t local_offset;
} table;

static void Table_push_scope()
{
    Scope* parent = table.local;
    table.local = (Scope*)malloc(sizeof(Scope));

    if (!table.local)
    {
        Panic(Memory_Error);
    }

    table.local->parent = parent;
    table.local->offset = (parent) ? parent->offset : 0;
    table.local->symbols = Map_create(sizeof(Symbol));
}

void Table_pop_scope()
{
    Scope* scope = table.local;
    table.local = table.local->parent;

    if (table.local_offset < scope->offset)
    {
        table.local_offset = scope->offset;
    }

    Map_destroy(&scope->symbols);
    free(scope);
}

Symbol* Table_get_symbol(String id)
{
    Scope* scope = table.local;

    while (scope)
    {
        Symbol* symbol = (Symbol*)Map_get(&scope->symbols, id);

        if (symbol)
        {
            return symbol;
        }

        scope = scope->parent;
    }

    return (Symbol*)(Map_get(&table.global, id));
}

Boolean Table_set_symbol(String id, Symbol symbol)
{
    static size_t function_last_id = 0;

    if (symbol.symbol_type != SYMBOL_LOCAL_VAR && Map_get(&table.global, id) ||
        symbol.symbol_type == SYMBOL_LOCAL_VAR &&
            Map_get(&table.local->symbols, id))
    {
        Error("KeyError: redefinition of symbol '%s'\n", String_begin(id));
        return False;
    }

    switch (symbol.symbol_type)
    {
        case SYMBOL_LOCAL_VAR:
        {
            size_t size = Type_size(symbol.type);
            size_t mask = Type_alignment(symbol.type) - 1;
            symbol.offset = (table.local->offset + mask) & ~mask;
            table.local->offset = symbol.offset + Type_size(symbol.type);
            Map_set(&table.local->symbols, id, &symbol);

            // Print("%s => local offset %llu\n", String_begin(id),
            // symbol.offset);
            break;
        }
        case SYMBOL_GLOBAL_VAR:
        {
            size_t size = Type_size(symbol.type);
            size_t mask = Type_alignment(symbol.type) - 1;
            symbol.offset = (table.global_offset + mask) & ~mask;
            table.global_offset = symbol.offset + Type_size(symbol.type);
            Map_set(&table.global, id, &symbol);

            // Print("%s => global offset %llu\n", String_begin(id),
            //    symbol.offset);
            //         break;
        }
        case SYMBOL_FUNCTION:
        {
            symbol.offset = function_last_id++;
            Map_set(&table.global, id, &symbol);

            // Print("%s => function offset %llu ", String_begin(id),
            //      symbol.offset);
            // Type_print(symbol.type);
            // Print("\n");
            break;
        }
        case SYMBOL_TYPE:
        {
            Map_set(&table.global, id, &symbol);

            // Print("%s => type ", String_begin(id));
            // Type_print(symbol.type);
            // Print("\n");
            break;
        }
    }

    return True;
}

// @Headers

void Parser_next_token(Parser* self);
void Parser_push_token(Parser* self, Token token);

AST* Parse(String path);

AST* Parse_file(String path);

Type* Parse_type(Parser* self);

AST* Parse_top(Parser* self);

AST* Parse_declare(Parser* self, Boolean global);
AST* Parse_function(Parser* self);
AST* Parse_block(Parser* self);
AST* Parse_statement(Parser* self);

AST_Expression* Parse_expression0(Parser* self);
AST_Expression* Parse_expression0_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression1(Parser* self);
AST_Expression* Parse_expression1_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression2(Parser* self);
AST_Expression* Parse_expression2_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression3(Parser* self);
AST_Expression* Parse_expression3_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression4(Parser* self);
AST_Expression* Parse_expression4_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression5(Parser* self);
AST_Expression* Parse_expression5_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression6(Parser* self);
AST_Expression* Parse_expression6_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression7(Parser* self);
AST_Expression* Parse_expression7_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression8(Parser* self);
AST_Expression* Parse_expression8_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression9(Parser* self);
AST_Expression* Parse_expression9_tail(Parser* self, AST_Expression* lhs);

AST_Expression* Parse_expression_prefix(Parser* self);
AST_Expression* Parse_expression_root(Parser* self);
AST_Expression* Parse_expression_postfix(Parser* self, AST_Expression* lhs);

// @Definitions

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

AST* Parse(String path)
{
    table.global = Map_create(sizeof(Symbol));
    table.local = NULL;

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

    Table_set_symbol(u8_type_name,
                     (Symbol){.type = type_u8, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(u16_type_name,
                     (Symbol){.type = type_u16, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(u32_type_name,
                     (Symbol){.type = type_u32, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(u64_type_name,
                     (Symbol){.type = type_u64, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(s8_type_name,
                     (Symbol){.type = type_s8, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(s16_type_name,
                     (Symbol){.type = type_s16, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(s32_type_name,
                     (Symbol){.type = type_s32, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(s64_type_name,
                     (Symbol){.type = type_s64, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(fp32_type_name,
                     (Symbol){.type = type_fp32, .symbol_type = SYMBOL_TYPE});
    Table_set_symbol(fp64_type_name,
                     (Symbol){.type = type_fp64, .symbol_type = SYMBOL_TYPE});

    AST* ast = Parse_file(path);
    ((AST_Program*)(ast))->data = table.global_offset;

    Map_destroy(&table.global);

    return ast;
}

AST* Parse_file(String path)
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

    AST_Program* ast = AST_Program_create();

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

        AST* statement = Parse_top(&self);

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
            AST_Program* ast2 = (AST_Program*)(statement);
            Stack_push_stack(&ast->statements, &ast2->statements);
        }
        else
        {
            Stack_push(&ast->statements, &statement);
        }
    }

    return (AST*)(ast);
}

Type* Parse_type(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case Tag_ID:
        {
            String id = self->token.lexeme;

            Symbol* symbol = Table_get_symbol(id);

            if (!symbol)
            {
                LogError(self->token, "%s is not defined", String_begin(id));
                return NULL;
            }

            if (symbol->symbol_type != SYMBOL_TYPE)
            {
                LogError(self->token, "%s is not a type", String_begin(id));
                return NULL;
            }

            return Type_copy(symbol->type);
        }
        case '*':
        {
            Type* value = Parse_type(self);

            if (!value)
            {
                return NULL;
            }

            Type_Pointer* type = Type_Pointer_create();
            type->value = value;
            return (Type*)(type);
        }
        case '[':
        {
            Parser_next_token(self);

            if (self->token.tag != Tag_LITERAL_INTEGER)
            {
                LogError(self->token, "Expected an 'Integer Literal'");
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != ']')
            {
                LogError(self->token, "Expected ']'");
                return NULL;
            }

            Type* value = Parse_type(self);

            U64 size = strtoull(String_begin(self->token.lexeme), NULL, 10);

            if (!value)
            {
                return NULL;
            }

            Type_Array* type = Type_Array_create();
            type->value = value;
            type->size = size;
            return (Type*)(type);
        }
        case '<':
        {
            Type_Tuple* tuple = Type_Tuple_create();

            Boolean failed = False;

            for (;;)
            {
                Type* type = Parse_type(self);

                if (!type)
                {
                    failed = True;
                }
                else
                {
                    Type_Tuple_push_field(tuple, type);
                }

                Parser_next_token(self);

                if (self->token.tag != ',')
                {
                    Parser_push_token(self, self->token);
                    break;
                }
            }

            Parser_next_token(self);

            if (self->token.tag != '>')
            {
                LogError(self->token, "Expected '>'");
                Type_destroy((Type*)(tuple));
                return NULL;
            }

            if (failed)
            {
                Type_destroy((Type*)(tuple));
                return NULL;
            }

            return (Type*)(tuple);
        }
        case '{':
        {
            Type_Struct* struct_ = Type_Struct_create();

            Boolean failed = False;

            for (;;)
            {
                Parser_next_token(self);

                if (self->token.tag != Tag_ID)
                {
                    LogError(self->token, "Expected an identifier");
                    Type_destroy((Type*)(struct_));
                    return NULL;
                }

                Token token = self->token;
                String id = self->token.lexeme;

                Parser_next_token(self);

                if (self->token.tag != ':')
                {
                    LogError(self->token, "Expected ':'");
                    Type_destroy((Type*)(struct_));
                    return NULL;
                }

                Type* type = Parse_type(self);

                if (Type_Struct_field_exists(struct_, id))
                {
                    LogError(token, "Field with name %s already exists",
                             String_begin(id));
                    failed = True;
                }
                else if (!type)
                {
                    failed = True;
                }
                else
                {
                    Type_Struct_push_field(struct_, id, type);
                }

                Parser_next_token(self);

                if (self->token.tag != ',')
                {
                    Parser_push_token(self, self->token);
                    break;
                }
            }

            Parser_next_token(self);

            if (self->token.tag != '}')
            {
                LogError(self->token, "Expected '}'");
                Type_destroy((Type*)(struct_));
                return NULL;
            }

            if (failed)
            {
                Type_destroy((Type*)(struct_));
                return NULL;
            }

            return (Type*)(struct_);
        }
        case '(':
        {
            return NULL;
        }
    }

    return NULL;
}

AST* Parse_top(Parser* self)
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

            return Parse_file(path);
        }
        case Tag_TYPE:
        {
            Type* type = Parse_type(self);

            Parser_next_token(self);

            if (self->token.tag != Tag_AS)
            {
                LogError(self->token, "Expected 'as'");
                Type_destroy(type);
                return NULL;
            }

            Parser_next_token(self);

            if (self->token.tag != Tag_ID)
            {
                LogError(self->token, "Expected an identifier");
                Type_destroy(type);
                return NULL;
            }

            String id = self->token.lexeme;

            Parser_next_token(self);

            if (self->token.tag != ';')
            {
                LogError(self->token, "Expected ';'");
                Type_destroy(type);
                return NULL;
            }

            if (type)
            {
                Table_set_symbol(
                    id, (Symbol){.type = type, .symbol_type = SYMBOL_TYPE});
            }

            return NULL;
        }
        case Tag_CONST:
        case Tag_VAR:
        {
            return Parse_declare(self, True);
        }
        case Tag_FUNCTION:
        {
            return Parse_function(self);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return Parse_statement(self);
        }
    }
}

AST* Parse_declare(Parser* self, Boolean is_global)
{
    Boolean constant = self->token.tag == Tag_CONST;

    Stack ids = Stack_create(sizeof(String));
    AST_Expression* initializer = NULL;
    Type* type = NULL;

    for (Boolean step = True; step;)
    {
        Parser_next_token(self);

        if (self->token.tag != Tag_ID)
        {
            LogError(self->token, "expected an identifier");
            return NULL;
        }

        Stack_push(&ids, &self->token.lexeme);

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

    if (is_global && self->token.tag == '=')
    {
        initializer = Parse_expression0(self);

        if (initializer)
        {
            return NULL;
        }

        Parser_next_token(self);
    }
    else
    {
        Parser_push_token(self, self->token);

        type = Parse_type(self);

        if (!type)
        {
            return NULL;
        }

        Parser_next_token(self);

        if (is_global && self->token.tag == '=')
        {
            initializer = Parse_expression0(self);

            if (!initializer)
            {
                return NULL;
            }

            Parser_next_token(self);
        }
    }

    if (self->token.tag != ';')
    {
        LogError(self->token, "Expected ';'");
        return NULL;
    }

    if (!Table_set_symbol(
            *(String*)(Stack_get(&ids, 0)),
            (Symbol){.type = type,
                     .symbol_type =
                         (is_global ? SYMBOL_GLOBAL_VAR : SYMBOL_LOCAL_VAR),
                     .constant = constant,
                     .initialized = False}))
    {
        return NULL;
    }

    return NULL;
}

AST* Parse_function(Parser* self)
{
    Parser_next_token(self);

    if (self->token.tag != Tag_ID)
    {
        LogError(self->token, "Expected an identifier");
        return NULL;
    }

    String id = self->token.lexeme;

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

        for (;;)
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

            if (self->token.tag != ':')
            {
                LogError(self->token, "Expected ':'");
                AST_destroy((AST*)(function));
                return NULL;
            }

            Type* type = Parse_type(self);

            if (!type)
            {
                AST_destroy((AST*)(function));
                return NULL;
            }

            Stack_push(&function->in_types, &type);

            Parser_next_token(self);

            if (self->token.tag != ',')
            {
                break;
            }
        }

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

        for (;;)
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

            if (self->token.tag != ':')
            {
                LogError(self->token, "Expected ':'");
                AST_destroy((AST*)(function));
                return NULL;
            }

            Type* type = Parse_type(self);

            if (!type)
            {
                AST_destroy((AST*)(function));
                return NULL;
            }

            Stack_push(&function->out_types, &type);

            Parser_next_token(self);

            if (self->token.tag != ',')
            {
                break;
            }
        }

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

    Type_Function* type = Type_Function_create();

    for (size_t i = 0; i < function->in_types.size; i++)
    {
        Type* field = Type_copy(*(Type**)Stack_get(&function->in_types, i));
        Stack_push(&type->inputs, &field);
    }

    if (function->out_types.size > 1)
    {
        Type_Tuple* tuple = Type_Tuple_create();

        for (size_t i = 0; i < function->out_types.size; i++)
        {
            Type* field =
                Type_copy(*(Type**)Stack_get(&function->out_types, i));

            Type_Tuple_push_field(tuple, field);
        }

        type->output = (Type*)(tuple);
    }
    else if (function->out_types.size == 1)
    {
        type->output = Type_copy(*(Type**)(Stack_get(&function->out_types, 0)));
    }

    Table_set_symbol(
        id, (Symbol){.type = (Type*)(type), .symbol_type = SYMBOL_FUNCTION});

    table.local_offset = 0;
    Table_push_scope();

    for (size_t i = 0; i < function->in_ids.size; i++)
    {
        Table_set_symbol(
            *(String*)(Stack_get(&function->in_ids, i)),
            (Symbol){.type = *(Type**)(Stack_get(&function->in_types, i)),
                     .symbol_type = SYMBOL_LOCAL_VAR});
    }

    for (size_t i = 0; i < function->out_ids.size; i++)
    {
        Table_set_symbol(
            *(String*)(Stack_get(&function->out_ids, i)),
            (Symbol){.type = *(Type**)(Stack_get(&function->out_types, i)),
                     .symbol_type = SYMBOL_LOCAL_VAR});
    }

    function->body = (AST*)(Parse_block(self));

    Table_pop_scope();

    if (!function->body)
    {
        AST_destroy((AST*)(function));
        return NULL;
    }

    function->id = id;
    function->data = table.local_offset;

    return (AST*)(function);
}

AST_Expression* Parse_expression0(Parser* self)
{
    return Parse_expression0_tail(self, Parse_expression1(self));
}

AST_Expression* Parse_expression0_tail(Parser* self, AST_Expression* lhs)
{
    return lhs;
}

AST_Expression* Parse_expression1(Parser* self)
{
    return Parse_expression1_tail(self, Parse_expression2(self));
}

AST_Expression* Parse_expression1_tail(Parser* self, AST_Expression* lhs)
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

                AST_Expression* rhs = Parse_expression2(self);

                if (!rhs)
                {
                    AST_destroy((AST*)(lhs));

                    return NULL;
                }

                Stack_push(&tuple->fields, &rhs);
            }
            else
            {
                AST_Expression* rhs = Parse_expression2(self);

                if (!rhs)
                {
                    AST_destroy((AST*)(lhs));

                    return NULL;
                }

                tuple = AST_Tuple_create();
                Stack_push(&tuple->fields, &lhs);
                Stack_push(&tuple->fields, &rhs);
            }

            return Parse_expression1_tail(self, (AST_Expression*)(tuple));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression2(Parser* self)
{
    return Parse_expression2_tail(self, Parse_expression3(self));
}

AST_Expression* Parse_expression2_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression3(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression2_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression3(Parser* self)
{
    return Parse_expression3_tail(self, Parse_expression4(self));
}

AST_Expression* Parse_expression3_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression4(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression3_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression4(Parser* self)
{
    return Parse_expression4_tail(self, Parse_expression5(self));
}

AST_Expression* Parse_expression4_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression5(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression4_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression5(Parser* self)
{
    return Parse_expression5_tail(self, Parse_expression6(self));
}

AST_Expression* Parse_expression5_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression6(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression5_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression6(Parser* self)
{
    return Parse_expression6_tail(self, Parse_expression7(self));
}

AST_Expression* Parse_expression6_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression7(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression6_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression7(Parser* self)
{
    return Parse_expression7_tail(self, Parse_expression8(self));
}

AST_Expression* Parse_expression7_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression8(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression7_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression8(Parser* self)
{
    return Parse_expression8_tail(self, Parse_expression9(self));
}

AST_Expression* Parse_expression8_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression9(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression8_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression9(Parser* self)
{
    return Parse_expression9_tail(self, Parse_expression_prefix(self));
}

AST_Expression* Parse_expression9_tail(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;
            int op = token.tag;

            AST_Expression* rhs = Parse_expression_prefix(self);

            if (!rhs)
            {
                return NULL;
            }

            if (!Type_equal(lhs->type, rhs->type))
            {
                LogError(token, "Incompatible Types");
                Print("  ");
                Type_print(lhs->type);
                Print(" %s ", Operators[token.tag]);
                Type_print(rhs->type);
                Print("\n");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            AST_Binary* binary = AST_Binary_create();
            binary->op = op;
            binary->type = Type_copy(lhs->type);
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Parse_expression9_tail(self, (AST_Expression*)(binary));
        }
        default:
        {
            Parser_push_token(self, self->token);
            return lhs;
        }
    }
}

AST_Expression* Parse_expression_prefix(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '*':
        {
            Token token = self->token;
            int op = token.tag;

            AST_Expression* expression =
                Parse_expression_postfix(self, Parse_expression2(self));

            if (!expression)
            {
                return NULL;
            }

            if (expression->AST_id != AST_STORAGE &&
                expression->AST_id != AST_ADDRESS)
            {
                LogError(token, "Expression doesn't have memory address");
                return NULL;
            }

            if (expression->AST_id == AST_ADDRESS)
            {
                return expression;
            }

            AST_Address* address = AST_Address_create();
            Type_Pointer* pointer = Type_Pointer_create();
            pointer->value = Type_copy(expression->type);
            address->type = (Type*)pointer;
            address->base = expression;
            return (AST_Expression*)(address);
        }
        case '!':
        case '-':
        case '~':
        {
            Token token = self->token;
            int op = token.tag;

            AST_Expression* expression =
                Parse_expression_postfix(self, Parse_expression2(self));

            if (!expression)
            {
                return NULL;
            }

            if (op == '-')
            {
                if (expression->type->type_id != TYPE_FLOAT &&
                    expression->type->type_id != TYPE_INTEGER)
                {
                    LogError(
                        token,
                        "Operator '%s' only apply to 'integers' and 'floating "
                        "points'",
                        Operators[op]);
                    return NULL;
                }
            }
            else if (expression->type->type_id != TYPE_INTEGER)
            {
                LogError(token, "Operator '%s' only apply to 'integers'",
                         Operators[op]);
                return NULL;
            }

            AST_Unary* unary = AST_Unary_create();
            unary->op = op;
            unary->type = Type_copy(expression->type);
            unary->expression = expression;
            return (AST_Expression*)(unary);
        }
        default:
        {
            Parser_push_token(self, self->token);
            return Parse_expression_postfix(self, Parse_expression_root(self));
        }
    }
}

AST_Expression* Parse_expression_root(Parser* self)
{
    Parser_next_token(self);

    switch (self->token.tag)
    {
        case '(':
        {
            AST_Expression* expression = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                LogError(self->token, "Expected ')'");
                AST_destroy((AST*)expression);
                return NULL;
            }

            if (!expression)
            {
                return NULL;
            }

            return expression;
        }
        case Tag_LITERAL_STRING:
        {
            AST_String_Literal* lit = AST_String_Literal_create();
            lit->value = self->token.lexeme;
            Type_Pointer* ptr = Type_Pointer_create();
            ptr->value = type_u8;
            lit->type = (Type*)ptr;
            return (AST_Expression*)lit;
        }
        case Tag_LITERAL_INTEGER:
        {
            AST_Integer_Literal* lit = AST_Integer_Literal_create();
            lit->value = self->token.lexeme;
            lit->type = (Type*)type_u64;
            return (AST_Expression*)lit;
        }
        case Tag_LITERAL_CHAR:
        {
            AST_Integer_Literal* lit = AST_Integer_Literal_create();
            lit->value =
                String_fmt("%" PRIu8 "", String_begin(self->token.lexeme)[0]);
            lit->type = (Type*)type_u8;
            return (AST_Expression*)lit;
        }
        case Tag_LITERAL_REAL:
        {
            AST_Real_Literal* lit = AST_Real_Literal_create();
            lit->value = self->token.lexeme;
            lit->type = (Type*)type_fp64;
            return (AST_Expression*)lit;
        }
        case Tag_ID:
        {
            String id = self->token.lexeme;
            Symbol* symbol = Table_get_symbol(id);

            if (!symbol)
            {
                LogError(self->token, "%s is not defined", String_begin(id));
                return NULL;
            }

            if (symbol->symbol_type == SYMBOL_TYPE)
            {
                LogError(self->token, "%s is a 'Type'");
                return NULL;
            }

            AST_Storage* storage = AST_Storage_create();
            storage->type = Type_copy(symbol->type);
            storage->offset = symbol->offset;
            storage->global = symbol->symbol_type == SYMBOL_GLOBAL_VAR ||
                              symbol->symbol_type == SYMBOL_FUNCTION;
            return (AST_Expression*)storage;
        }
        default:
        {
            LogError(self->token, "Invalid expression");
            return NULL;
        }
    }
}

AST_Expression* Parse_expression_postfix(Parser* self, AST_Expression* lhs)
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
            Token token = self->token;

            AST_Expression* rhs = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag != ']')
            {
                AST_destroy((AST*)rhs);
                return NULL;
            }

            if (!rhs)
            {
                return NULL;
            }

            if (lhs->AST_id != AST_STORAGE && lhs->AST_id != AST_ADDRESS)
            {
                LogError(token, "Expression is not subscriptable");
                AST_destroy((AST*)rhs);
                return NULL;
            }

            switch (lhs->type->type_id)
            {
                case TYPE_POINTER:
                case TYPE_ARRAY:
                {
                    Type_Pointer* type = (Type_Pointer*)(lhs->type);

                    AST_Address* address = AST_Address_create();
                    address->base = lhs;
                    address->type = type->value;

                    if (rhs->AST_id == AST_INTEGER_LITERAL)
                    {
                        AST_Integer_Literal* lit = (AST_Integer_Literal*)(rhs);

                        size_t offset =
                            strtoull(String_begin(lit->value), NULL, 10);

                        lit->value =
                            String_fmt("%llu", Type_size(type->value) * offset);

                        address->offset = rhs;
                    }
                    else
                    {
                        AST_Binary* offset = AST_Binary_create();
                        AST_Integer_Literal* lit = AST_Integer_Literal_create();
                        lit->value = String_fmt("%llu", Type_size(type->value));
                        offset->op = '*';
                        offset->lhs = rhs;
                        offset->rhs = (AST_Expression*)lit;
                        address->offset = (AST_Expression*)(offset);
                    }

                    return Parse_expression_postfix(self,
                                                    (AST_Expression*)address);
                }
                case TYPE_TUPLE:
                {
                    Type_Tuple* tuple = (Type_Tuple*)(lhs->type);

                    if (rhs->AST_id != AST_INTEGER_LITERAL)
                    {
                        LogError(token,
                                 "Tuple subscripting only accepts 'integer "
                                 "literal'");
                        break;
                    }

                    AST_Integer_Literal* lit = (AST_Integer_Literal*)rhs;

                    if (lhs->AST_id == AST_STORAGE)
                    {
                        AST_Storage* storage = (AST_Storage*)(lhs);

                        size_t index =
                            strtoull(String_begin(lit->value), NULL, 10);

                        if (index > tuple->fields.size)
                        {
                            LogError(token, "Index out of range");
                            break;
                        }

                        Type* type = Type_copy(
                            *(Type**)(Stack_get(&tuple->fields, index)));

                        size_t offset =
                            *(size_t*)(Stack_get(&tuple->offsets, index));

                        Type_destroy(lhs->type);
                        storage->type = type;
                        storage->offset += offset;

                        return Parse_expression_postfix(self, lhs);
                    }
                    else
                    {
                        AST_Address* address = (AST_Address*)(lhs);

                        size_t index =
                            strtoull(String_begin(lit->value), NULL, 10);

                        if (index > tuple->fields.size)
                        {
                            LogError(token, "Index out of range");
                            break;
                        }

                        Type* type = Type_copy(
                            *(Type**)(Stack_get(&tuple->fields, index)));

                        size_t offset =
                            *(size_t*)(Stack_get(&tuple->offsets, index));

                        Type_destroy(lhs->type);
                        address->type = type;

                        if (address->offset->AST_id == AST_INTEGER_LITERAL)
                        {
                            AST_Integer_Literal* lit =
                                (AST_Integer_Literal*)(address->offset);

                            size_t lit_offset =
                                strtoull(String_begin(lit->value), NULL, 10);

                            lit->value =
                                String_fmt("%llu", lit_offset + offset);

                            address->offset = (AST_Expression*)(lit);
                        }
                        else
                        {
                            AST_Binary* bin = AST_Binary_create();
                            AST_Integer_Literal* lit =
                                AST_Integer_Literal_create();
                            lit->value = String_fmt("%llu", offset);
                            bin->op = '*';
                            bin->lhs = address->offset;
                            bin->rhs = (AST_Expression*)lit;
                            address->offset = (AST_Expression*)(bin);
                        }

                        return Parse_expression_postfix(self, lhs);
                    }
                }
                default:
                {
                    LogError(token, "Expression is not subscriptable");
                    AST_destroy((AST*)rhs);
                    return NULL;
                }
            }
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
            AST_If* if_ = AST_If_create();

            Parser_next_token(self);

            if (self->token.tag != '(')
            {
                LogError(self->token, "Expected '('");
                return NULL;
            }

            if_->condition = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                LogError(self->token, "Expected ')'");
                AST_destroy((AST*)(if_));
                return NULL;
            }

            if_->then = Parse_statement(self);

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

            if (!if_->condition)
            {
                AST_destroy((AST*)(if_));
                return NULL;
            }

            if (!if_->then)
            {
                AST_destroy((AST*)(if_));
                return NULL;
            }

            return (AST*)(if_);
        }
        case Tag_WHILE:
        {
            AST_While* while_ = AST_While_create();

            Parser_next_token(self);

            if (self->token.tag != '(')
            {
                LogError(self->token, "Expected '('");
                return NULL;
            }

            while_->condition = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag != ')')
            {
                LogError(self->token, "Expected ')'");
                AST_destroy((AST*)(while_));
                return NULL;
            }

            while_->then = Parse_statement(self);

            if (!while_->condition)
            {
                AST_destroy((AST*)(while_));
                return NULL;
            }

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
            return Parse_declare(self, False);
        }
        case '{':
        {
            return (AST*)(Parse_block(self));
        }
        default:
        {
            Parser_push_token(self, self->token);

            AST_Expression* expression = Parse_expression0(self);

            Parser_next_token(self);

            if (self->token.tag == '=')
            {
                AST_Expression* rhs = Parse_expression0(self);

                Parser_next_token(self);

                if (self->token.tag != ';')
                {
                    LogError(self->token, "Expected ';'");
                    AST_destroy((AST*)expression);
                    return NULL;
                }

                if (!expression || !rhs)
                {
                    AST_destroy((AST*)expression);
                    return NULL;
                }

                AST_Assign* assign = AST_Assign_create();
                assign->lhs = expression;
                assign->rhs = rhs;

                return (AST*)(assign);
            }
            else
            {
                if (self->token.tag != ';')
                {
                    LogError(self->token, "Expected ';'");
                    AST_destroy((AST*)expression);
                    return NULL;
                }

                if (!expression)
                {
                    AST_destroy((AST*)expression);
                    return NULL;
                }

                return (AST*)(expression);
            }
        }
    }
}

AST* Parse_block(Parser* self)
{
    AST_Block* block = AST_Block_create();
    Table_push_scope();

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

    Table_pop_scope();
    return (AST*)(block);
}
