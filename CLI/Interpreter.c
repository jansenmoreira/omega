/*
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

void Interpreter_top_level(Interpreter* self);

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

        Interpreter_top_level(&self);

        free(line);
    }

    Lexer_free();
}
*/
