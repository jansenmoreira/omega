#include <Language/Interpreter.h>

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
static String type_type_name;

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

Interpreter Interpreter_create()
{
    Interpreter self;
    self.machine = Machine_create();
    self.scopes.local = NULL;
    Interpreter_push_scope(&self);
    self.scopes.global = self.scopes.local;

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

    return self;
}

static void LogError(AST* node, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    Error("SemanticError: ");

    Error_va(format, args);

    Error("\n");

    va_end(args);
}

Type* Interpreter_evaluate_type(Interpreter* self, AST* ast)
{
    switch (ast->AST_id)
    {
        case AST_UNARY:
        {
            AST_Unary* unary = (AST_Unary*)(ast);

            if (unary->op != '*')
            {
                return NULL;
            }

            Type* value_type =
                Interpreter_evaluate_type(self, unary->expression);

            if (!value_type)
            {
                return NULL;
            }

            Type_Pointer* pointer = Type_Pointer_create();
            pointer->type = value_type;
            return (Type*)(pointer);
        }
        case AST_MAP:
        {
            AST_Map* map = (AST_Map*)(ast);
            Type_Function* function_t = Type_Function_create();

            function_t->input = Interpreter_evaluate_type(self, map->input);

            if (!function_t->input)
            {
                Type_destroy(function_t);
                return NULL;
            }

            function_t->output = Interpreter_evaluate_type(self, map->output);

            if (!function_t->output)
            {
                Type_destroy(function_t);
                return NULL;
            }

            return function_t;
        }
        case AST_TUPLE:
        {
            AST_Tuple* tuple = (AST_Tuple*)(ast);
            Type_Tuple* tuple_t = Type_Tuple_create();

            for (size_t i = 0; i < tuple->fields.size; i++)
            {
                Type* field_type = Interpreter_evaluate_type(
                    self, *(AST**)(Stack_get(&tuple->fields, i)));

                if (!field_type)
                {
                    Type_destroy((Type*)(tuple_t));
                    return NULL;
                }

                Stack_push(&tuple_t->fields, &field_type);
            }

            return (Type*)(tuple_t);
        }
        case AST_REFERENCE:
        {
            AST_Reference* reference = (AST_Reference*)(ast);
            Symbol* symbol = Interpreter_get_symbol(self, reference->id);

            if (!symbol)
            {
                LogError(ast, "'%s' is not defined", reference->id);
                return NULL;
            }

            if (!Type_equal(symbol->type, type_type))
            {
                LogError(ast, "'%s' is not a 'type'", reference->id);
                return NULL;
            }

            return (Type*)(symbol->storage);
        }
    }

    return NULL;
}

void Interpreter_run(Interpreter* self, AST* ast)
{
    switch (ast->AST_id)
    {
        case AST_PROGRAM:
        {
            AST_Program* program = (AST_Program*)(ast);

            for (size_t i = 0; i < program->statements.size; i++)
            {
                Interpreter_run(self,
                                *(AST**)(Stack_get(&program->statements, i)));
            }

            break;
        }

        case AST_DECLARE:
        {
            AST_Declare* declare = (AST_Declare*)(ast);

            if (declare->type)
            {
                Type* type = Interpreter_evaluate_type(self, declare->type);

                if (type)
                {
                    Type_print(type);
                    Print("\n");
                }
            }

            break;
        }
    }
}
