#include <Interpreter/Interpreter.h>

typedef struct Scope
{
    struct Scope *parent;
    MAP(size_t) table;
    STACK(u8) values;
} Scope;

Scope Scope_Create(Scope *parent)
{
    Scope self;
    self.parent = parent;
    self.table = MAP_CREATE(size_t);
    self.values = STACK_CREATE(u8);
    return self;
}

void Scope_destroy(Scope *self)
{
    MAP_DESTROY(size_t, self->table);
    STACK_DESTROY(u8, self->values);
}

void *Scope_get(Scope *self, String id)
{
    do
    {
        size_t *index = MAP_GET(size_t, self->table, id);

        if (!index) continue;

        return self->values.buffer + *index;
    } while (self->parent);

    return NULL;
}

typedef struct Interpreter
{
    Lexer *lexer;
    Token token;

    Scope *global;
    Scope *actual;
    STACK(u8) stack;
    MAP(Type *) types;

    STACK(Token) queue;
    size_t queue_it;
} Interpreter;

void Interpreter_next_token(Interpreter *self);

Type *Interpreter_parse_type(Interpreter *self);

void Interpreter_global_declaration(Interpreter *self);

int Interpreter_type_declaration(Interpreter *self);

int Interpreter_variable_declaration(Interpreter *self);

Expression *Interpreter_parse_expression0(Interpreter *self);
Expression *Interpreter_parse_expression0_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression1(Interpreter *self);
Expression *Interpreter_parse_expression1_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression2(Interpreter *self);
Expression *Interpreter_parse_expression2_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression3(Interpreter *self);
Expression *Interpreter_parse_expression3_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression4(Interpreter *self);
Expression *Interpreter_parse_expression4_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression5(Interpreter *self);
Expression *Interpreter_parse_expression5_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression6(Interpreter *self);
Expression *Interpreter_parse_expression6_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression7(Interpreter *self);
Expression *Interpreter_parse_expression7_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression8(Interpreter *self);
Expression *Interpreter_parse_expression8_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression9(Interpreter *self);
Expression *Interpreter_parse_expression9_tail(Interpreter *self,
                                               Expression *lhs);

Expression *Interpreter_parse_expression_prefix(Interpreter *self);

Expression *Interpreter_parse_expression_root(Interpreter *self);

int Interpreter_evaluate(Interpreter *self, Expression *expression);

String id_u8;
String id_u16;
String id_u32;
String id_u64;
String id_s8;
String id_s16;
String id_s32;
String id_s64;
String id_fp32;
String id_fp64;

void Interpreter_next_token(Interpreter *self)
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

void Interpreter_push_token(Interpreter *self, Token token)
{
    STACK_PUSH(Token, self->queue, token);
}

void Interpreter_scope_get(Interpreter *self, String id, Type **type,
                           void **value)
{
    Scope *scope = self->actual;

    while (scope)
    {
        size_t *index = MAP_GET(size_t, scope->table, id);

        if (index)
        {
            *type = *((Type **)(self->global->values.buffer + *index));

            *value = ((void *)(self->global->values.buffer + *index +
                               sizeof(Type *)));

            return;
        }

        scope = self->actual->parent;
    }

    type = NULL;
    value = NULL;
}

void Interpreter_scope_add(Interpreter *self, String id, Type *type,
                           void *value)
{
    size_t index = self->actual->values.size;

    size_t amount = sizeof(Type *) + Type_size(type);
    STACK_GROW(u8, self->actual->values, amount);

    MAP_SET(size_t, self->global->table, id, index);

    if (value)
    {
        void *value_dst =
            ((void *)(self->global->values.buffer + index + sizeof(Type *)));

        memcpy(value_dst, value, Type_size(type));
    }
}

void Interpreter_scope_assign(Interpreter *self, String id, void *value)
{
    Type *type = NULL;
    void *value_dst = NULL;

    Interpreter_scope_get(self, id, &type, &value_dst);

    if (type && value_dst)
    {
        memcpy(value_dst, value, Type_size(type));
    }
}

void Interpreter_stack_get(Interpreter *self, Type **type, void **value)
{
    *type =
        *((Type **)(self->stack.buffer + self->stack.size - sizeof(Type *)));

    *value = (void *)(self->stack.buffer + self->stack.size - sizeof(Type *) -
                      Type_size(*type));
}

void Interpreter_stack_get2(Interpreter *self, Type **type_lhs,
                            void **value_lhs, Type **type_rhs, void **value_rhs)
{
    *type_rhs =
        *((Type **)(self->stack.buffer + self->stack.size - sizeof(Type *)));

    *value_rhs = (void *)(self->stack.buffer + self->stack.size -
                          sizeof(Type *) - Type_size(*type_rhs));

    *type_lhs =
        *((Type **)(self->stack.buffer + self->stack.size - sizeof(Type *) -
                    Type_size(*type_rhs) - sizeof(Type *)));

    *value_lhs =
        (void *)(self->stack.buffer + self->stack.size - sizeof(Type *) -
                 Type_size(*type_rhs) - sizeof(Type *) - Type_size(*type_lhs));
}

void Interpreter_stack_destroy_n(Interpreter *self, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        Type *type;
        void *value;

        Interpreter_stack_get(self, &type, &value);

        size_t amount = sizeof(Type *) + Type_size(type);
        STACK_SHRINK(u8, self->stack, amount);
    }
}

void Interpreter_stack_push(Interpreter *self, Type *type, void *value)
{
    size_t stack_size = self->stack.size;

    size_t amount = Type_size(type) + sizeof(Type *);
    STACK_GROW(u8, self->stack, amount);

    memcpy(self->stack.buffer + stack_size, value, Type_size(type));
    memcpy(self->stack.buffer + stack_size + Type_size(type), &type,
           sizeof(Type *));
}

void Interpreter_loop()
{
    printf("Omega Interpreter for Windows x86-64\n");

    Interpreter interpreter;

    Scope global = Scope_Create(NULL);
    interpreter.global = &global;
    interpreter.actual = &global;
    interpreter.stack = STACK_CREATE(u8);
    interpreter.types = MAP_CREATE(Type *);
    interpreter.queue = STACK_CREATE(Token);
    interpreter.queue_it = 0;

    id_u8 = String_new("u8", 2);
    Type *t_u8 = (Type *)Type_Integer_create(1, 0);
    MAP_SET(Type *, interpreter.types, id_u8, t_u8);

    id_u16 = String_new("u16", 3);
    Type *t_u16 = (Type *)Type_Integer_create(2, 0);
    MAP_SET(Type *, interpreter.types, id_u16, t_u16);

    id_u32 = String_new("u32", 3);
    Type *t_u32 = (Type *)Type_Integer_create(4, 0);
    MAP_SET(Type *, interpreter.types, id_u32, t_u32);

    id_u64 = String_new("u64", 3);
    Type *t_u64 = (Type *)Type_Integer_create(8, 0);
    MAP_SET(Type *, interpreter.types, id_u64, t_u64);

    id_s8 = String_new("s8", 2);
    Type *t_s8 = (Type *)Type_Integer_create(1, 1);
    MAP_SET(Type *, interpreter.types, id_s8, t_s8);

    id_s16 = String_new("s16", 3);
    Type *t_s16 = (Type *)Type_Integer_create(2, 1);
    MAP_SET(Type *, interpreter.types, id_s16, t_s16);

    id_s32 = String_new("s32", 3);
    Type *t_s32 = (Type *)Type_Integer_create(4, 1);
    MAP_SET(Type *, interpreter.types, id_s32, t_s32);

    id_s64 = String_new("s64", 3);
    Type *t_s64 = (Type *)Type_Integer_create(8, 1);
    MAP_SET(Type *, interpreter.types, id_s64, t_s64);

    id_fp32 = String_new("fp32", 4);
    Type *t_fp32 = (Type *)Type_Float_create(4);
    MAP_SET(Type *, interpreter.types, id_fp32, t_fp32);

    id_fp64 = String_new("fp64", 4);
    Type *t_fp64 = (Type *)Type_Float_create(8);
    MAP_SET(Type *, interpreter.types, id_fp64, t_fp64);

    for (int step = 1; step;)
    {
        printf(">>> ");

        char *line = Read();

        Lexer lexer = Lexer_create(line, String_new(NULL, 0));
        interpreter.lexer = &lexer;
        Interpreter_global_declaration(&interpreter);

        STACK_CLEAR(Token, interpreter.queue);
        interpreter.queue_it = 0;
        free(line);
        Lexer_destroy(&lexer);
    }
}

void Interpreter_global_declaration(Interpreter *self)
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
                Expression *exp = Interpreter_parse_expression0(self);

                if (exp && Interpreter_evaluate(self, exp))
                {
                    Type *type;
                    void *value;

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

int Interpreter_type_declaration(Interpreter *self)
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
        Type *n = NULL;
        MAP_SET(Type *, self->types, id, n);
        return 1;
    }

    if (self->token.tag != '{')
    {
        return 0;
    }

    Type_Struct *type = Type_Struct_create();

    for (int step = 1; step;)
    {
        Interpreter_next_token(self);

        if (self->token.tag != Tag_ID)
        {
            Type_destroy((Type *)type);
            return 0;
        }

        String field_id = self->token.lexeme;

        Interpreter_next_token(self);

        if (self->token.tag != ':')
        {
            Type_destroy((Type *)type);
            return 0;
        }

        Type *field_type = Interpreter_parse_type(self);

        if (!type)
        {
            Type_destroy((Type *)type);
            return 0;
        }

        STACK_PUSH(Type *, type->fields, field_type);
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
                Type_destroy((Type *)type);
                return 0;
            }
        }
    }

    Interpreter_next_token(self);

    if (self->token.tag != ';')
    {
        Type_destroy((Type *)type);
        return 0;
    }

    MAP_SET(Type *, self->types, id, type);

    Type_print((Type *)type);
    puts("");

    return 1;
}

Type *Interpreter_parse_type(Interpreter *self)
{
    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case Tag_ID:
        {
            Type **type = MAP_GET(Type *, self->types, self->token.lexeme);

            if (!type)
            {
                printf("Type Error: Type %s is undefined\n",
                       String_begin(self->token.lexeme));
                return NULL;
            }

            return (Type *)Type_Alias_create(self->token.lexeme, (Type *)*type);
        }
        case '*':
        {
            Type *type = Interpreter_parse_type(self);

            if (!type)
            {
                return NULL;
            }

            return (Type *)Type_Pointer_create(type);
        }
        case '[':
        {
            Interpreter_next_token(self);

            if (self->token.tag != Tag_LITERAL_INTEGER)
            {
                Interpreter_push_token(self, self->token);

                Type_Tuple *tuple = Type_Tuple_create();

                for (int step = 1; step;)
                {
                    Type *type = Interpreter_parse_type(self);

                    if (!type)
                    {
                        Type_destroy((Type *)tuple);
                        return NULL;
                    }

                    STACK_PUSH(Type *, tuple->fields, type);

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
                            Type_destroy((Type *)tuple);
                            return NULL;
                        }
                    }
                }

                return (Type *)tuple;
            }

            u64 size = strtoull(String_begin(self->token.lexeme), NULL, 10);

            Interpreter_next_token(self);

            if (self->token.tag != ']')
            {
                return NULL;
            }

            Type *type = Interpreter_parse_type(self);

            if (!type)
            {
                return NULL;
            }

            return (Type *)Type_Array_create(type, size);
        }
        case '(':
        {
            Type_Function *function = Type_Function_create();

            Interpreter_next_token(self);

            if (self->token.tag != Tag_VOID)
            {
                Interpreter_push_token(self, self->token);

                for (int step = 1; step;)
                {
                    Type *type = Interpreter_parse_type(self);

                    if (!type)
                    {
                        Type_destroy((Type *)function);
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
                            Type_destroy((Type *)function);
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
                    Type_destroy((Type *)function);
                    return NULL;
                }
            }

            Interpreter_next_token(self);

            if (self->token.tag != Tag_VOID)
            {
                Interpreter_push_token(self, self->token);

                Type_Tuple *tuple = Type_Tuple_create();

                for (int step = 1; step;)
                {
                    Type *type = Interpreter_parse_type(self);

                    if (!type)
                    {
                        Type_destroy((Type *)tuple);
                        Type_destroy((Type *)function);
                        return NULL;
                    }

                    STACK_PUSH(Type *, tuple->fields, type);

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
                            Type_destroy((Type *)tuple);
                            Type_destroy((Type *)function);
                            return NULL;
                        }
                    }
                }

                function->return_type = (Type *)tuple;
            }
            else
            {
                Interpreter_next_token(self);

                if (self->token.tag != ')')
                {
                    Type_destroy((Type *)function);
                    return NULL;
                }

                function->return_type = NULL;
            }

            return (Type *)function;
        }
        default:
        {
            return NULL;
        }
    }
}

int Interpreter_variable_declaration(Interpreter *self)
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

    Type *type = Interpreter_parse_type(self);

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

    size_t amount = sizeof(Type *) + Type_size(type);
    STACK_GROW(u8, self->global->values, amount);

    Type **ptr = (Type **)(self->global->values.buffer + index);
    *ptr = type;

    MAP_SET(size_t, self->global->table, id, index);

    return 1;
}

Expression *Interpreter_parse_expression0(Interpreter *self)
{
    return Interpreter_parse_expression0_tail(
        self, Interpreter_parse_expression2(self));
}

Expression *Interpreter_parse_expression0_tail(Interpreter *self,
                                               Expression *lhs)
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
            Expression *rhs = Interpreter_parse_expression2(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Assign *assign = Expression_Assign_create();
            assign->lhs = lhs;
            assign->rhs = rhs;
            return (Expression *)assign;
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression2(Interpreter *self)
{
    return Interpreter_parse_expression2_tail(
        self, Interpreter_parse_expression3(self));
}

Expression *Interpreter_parse_expression2_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression3(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression2_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression3(Interpreter *self)
{
    return Interpreter_parse_expression3_tail(
        self, Interpreter_parse_expression4(self));
}

Expression *Interpreter_parse_expression3_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression4(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression3_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression4(Interpreter *self)
{
    return Interpreter_parse_expression4_tail(
        self, Interpreter_parse_expression5(self));
}

Expression *Interpreter_parse_expression4_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression5(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression4_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression5(Interpreter *self)
{
    return Interpreter_parse_expression5_tail(
        self, Interpreter_parse_expression6(self));
}

Expression *Interpreter_parse_expression5_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression6(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression5_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression6(Interpreter *self)
{
    return Interpreter_parse_expression6_tail(
        self, Interpreter_parse_expression7(self));
}

Expression *Interpreter_parse_expression6_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression7(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression6_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression7(Interpreter *self)
{
    return Interpreter_parse_expression7_tail(
        self, Interpreter_parse_expression8(self));
}

Expression *Interpreter_parse_expression7_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression8(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression7_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression8(Interpreter *self)
{
    return Interpreter_parse_expression8_tail(
        self, Interpreter_parse_expression9(self));
}

Expression *Interpreter_parse_expression8_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression9(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression8_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression9(Interpreter *self)
{
    return Interpreter_parse_expression9_tail(
        self, Interpreter_parse_expression_prefix(self));
}

Expression *Interpreter_parse_expression9_tail(Interpreter *self,
                                               Expression *lhs)
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

            Expression *rhs = Interpreter_parse_expression_prefix(self);

            if (!rhs)
            {
                return NULL;
            }

            Expression_Binary *binary = Expression_Binary_create();
            binary->op = op;
            binary->lhs = lhs;
            binary->rhs = rhs;
            return Interpreter_parse_expression9_tail(self,
                                                      (Expression *)binary);
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return lhs;
        }
    }
}

Expression *Interpreter_parse_expression_prefix(Interpreter *self)
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

            Expression *expression = Interpreter_parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Expression_Unary *unary = Expression_Unary_create();
            unary->op = op;
            unary->expression = expression;
            return (Expression *)unary;
        }
        default:
        {
            Interpreter_push_token(self, self->token);
            return Interpreter_parse_expression_root(self);
        }
    }
}

Expression *Interpreter_parse_expression_root(Interpreter *self)
{
    Interpreter_next_token(self);

    switch (self->token.tag)
    {
        case '(':
        {
            Expression *expression = Interpreter_parse_expression0(self);

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

            Type *type = Interpreter_parse_type(self);

            if (!type)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->token.tag != ',')
            {
                return NULL;
            }

            Expression *expression = Interpreter_parse_expression0(self);

            if (!expression)
            {
                return NULL;
            }

            Interpreter_next_token(self);

            if (self->token.tag != ')')
            {
                return NULL;
            }

            Expression_Cast *cast = Expression_Cast_create();
            cast->type = type;
            cast->expression = expression;
            return (Expression *)cast;
        }
        case Tag_LITERAL_INTEGER:
        {
            Expression_Integer_Literal *lit =
                Expression_Integer_Literal_create();
            lit->value = self->token.lexeme;
            return (Expression *)lit;
        }
        case Tag_LITERAL_REAL:
        {
            Expression_Real_Literal *lit = Expression_Real_Literal_create();
            lit->value = self->token.lexeme;
            return (Expression *)lit;
        }
        case Tag_ID:
        {
            Expression_Reference *ref = Expression_Reference_create();
            ref->id = self->token.lexeme;
            return (Expression *)ref;
        }
        default:
        {
            printf("Expected an valid expression.\n");
            return NULL;
        }
    }
}

int Interpreter_evaluate_get_address(Interpreter *self, Expression *expression)
{
    switch (expression->expression_id)
    {
        case EXPRESSION_REFERENCE:
        {
            Expression_Reference *ref = (Expression_Reference *)expression;

            Type *type;
            void *value;

            Interpreter_scope_get(self, ref->id, &type, &value);

            Type_Pointer *pointer = Type_Pointer_create(type);

            Interpreter_stack_push(self, (Type *)pointer, value);
            break;
        }
        default:
        {
            return 0;
        }
    }

    return 1;
}

int Interpreter_evaluate(Interpreter *self, Expression *expression)
{
    switch (expression->expression_id)
    {
        case EXPRESSION_CAST:
        {
            Expression_Cast *cast = (Expression_Cast *)expression;

            if (!Interpreter_evaluate(self, cast->expression))
            {
                return 0;
            }

            Type *type;
            void *value;

            Interpreter_stack_get(self, &type, &value);

            Type *original_type = Type_dereference(type);
            Type *cast_to = Type_dereference(cast->type);

            switch (cast_to->type_id)
            {
                case TYPE_INTEGER:
                {
                    Type_Integer *cast_to_int = (Type_Integer *)cast_to;

                    switch (cast_to_int->size)
                    {
                        case 1:
                        {
                            switch (original_type->type_id)
                            {
                                case TYPE_INTEGER:
                                {
                                    Type_Integer *original_type_int =
                                        (Type_Integer *)original_type;

                                    switch (original_type_int->size)
                                    {
                                        case 1:
                                        {
                                            u8 res =
                                                (original_type_int->is_signed)
                                                    ? (s8)(*(s8 *)(value))
                                                    : (u8)(*(u8 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 2:
                                        {
                                            u8 res =
                                                (original_type_int->is_signed)
                                                    ? (s8)(*(s16 *)(value))
                                                    : (u8)(*(u16 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 4:
                                        {
                                            u8 res =
                                                (original_type_int->is_signed)
                                                    ? (s8)(*(s32 *)(value))
                                                    : (u8)(*(u32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            u8 res =
                                                (original_type_int->is_signed)
                                                    ? (s8)(*(s64 *)(value))
                                                    : (u8)(*(u64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_FLOAT:
                                {
                                    Type_Float *original_type_fp =
                                        (Type_Float *)original_type;

                                    switch (original_type_fp->size)
                                    {
                                        case 4:
                                        {
                                            s64 to_int =
                                                (s64)(*(fp32 *)(value));

                                            u8 res =
                                                (cast_to_int->is_signed)
                                                    ? ((to_int > 127)
                                                           ? 127
                                                           : ((to_int < -128)
                                                                  ? 128
                                                                  : (u8)to_int))
                                                    : ((to_int > 255)
                                                           ? 255
                                                           : ((to_int < 0)
                                                                  ? 0
                                                                  : (u8)to_int));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            s64 to_int =
                                                (s64)(*(fp64 *)(value));

                                            u8 res =
                                                (cast_to_int->is_signed)
                                                    ? ((to_int > 127)
                                                           ? 127
                                                           : ((to_int < -128)
                                                                  ? 128
                                                                  : (u8)to_int))
                                                    : ((to_int > 255)
                                                           ? 255
                                                           : ((to_int < 0)
                                                                  ? 0
                                                                  : (u8)to_int));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_POINTER:
                                {
                                    Type_Pointer *original_type_fp =
                                        (Type_Pointer *)original_type;

                                    u8 res = (u8)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                                case TYPE_FUNCTION:
                                {
                                    Type_Function *original_type_fp =
                                        (Type_Function *)original_type;

                                    u8 res = (u8)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                            }

                            break;
                        }
                        case 2:
                        {
                            switch (original_type->type_id)
                            {
                                case TYPE_INTEGER:
                                {
                                    Type_Integer *original_type_int =
                                        (Type_Integer *)original_type;

                                    switch (original_type_int->size)
                                    {
                                        case 1:
                                        {
                                            u16 res =
                                                (original_type_int->is_signed)
                                                    ? (s16)(*(s8 *)(value))
                                                    : (u16)(*(u8 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 2:
                                        {
                                            u16 res =
                                                (original_type_int->is_signed)
                                                    ? (s16)(*(s16 *)(value))
                                                    : (u16)(*(u16 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 4:
                                        {
                                            u16 res =
                                                (original_type_int->is_signed)
                                                    ? (s16)(*(s32 *)(value))
                                                    : (u16)(*(u32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            u16 res =
                                                (original_type_int->is_signed)
                                                    ? (s16)(*(s64 *)(value))
                                                    : (u16)(*(u64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_FLOAT:
                                {
                                    Type_Float *original_type_fp =
                                        (Type_Float *)original_type;

                                    switch (original_type_fp->size)
                                    {
                                        case 4:
                                        {
                                            s32 to_int =
                                                (s32)(*(fp32 *)(value));

                                            u16 res =
                                                (cast_to_int->is_signed)
                                                    ? ((to_int > 127)
                                                           ? 127
                                                           : ((to_int < -128)
                                                                  ? 128
                                                                  : (u16)
                                                                        to_int))
                                                    : ((to_int > 255)
                                                           ? 255
                                                           : ((to_int < 0)
                                                                  ? 0
                                                                  : (u16)
                                                                        to_int));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            s64 to_int =
                                                (s64)(*(fp64 *)(value));

                                            u16 res =
                                                (cast_to_int->is_signed)
                                                    ? (to_int > 32767
                                                           ? 32767
                                                           : to_int < -32768
                                                                 ? -32768
                                                                 : (u16)to_int)
                                                    : (to_int > 65535
                                                           ? 65535
                                                           : to_int < 0
                                                                 ? 0
                                                                 : (u16)to_int);

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_POINTER:
                                {
                                    Type_Pointer *original_type_fp =
                                        (Type_Pointer *)original_type;

                                    u16 res = (u16)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                                case TYPE_FUNCTION:
                                {
                                    Type_Function *original_type_fp =
                                        (Type_Function *)original_type;

                                    u16 res = (u16)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                            }

                            break;
                        }
                        case 4:
                        {
                            switch (original_type->type_id)
                            {
                                case TYPE_INTEGER:
                                {
                                    Type_Integer *original_type_int =
                                        (Type_Integer *)original_type;

                                    switch (original_type_int->size)
                                    {
                                        case 1:
                                        {
                                            u32 res =
                                                (original_type_int->is_signed)
                                                    ? (s32)(*(s8 *)(value))
                                                    : (u32)(*(u8 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 2:
                                        {
                                            u32 res =
                                                (original_type_int->is_signed)
                                                    ? (s32)(*(s16 *)(value))
                                                    : (u32)(*(u16 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 4:
                                        {
                                            u32 res =
                                                (original_type_int->is_signed)
                                                    ? (s32)(*(s32 *)(value))
                                                    : (u32)(*(u32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            u32 res =
                                                (original_type_int->is_signed)
                                                    ? (s32)(*(s64 *)(value))
                                                    : (u32)(*(u64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_FLOAT:
                                {
                                    Type_Float *original_type_fp =
                                        (Type_Float *)original_type;

                                    switch (original_type_fp->size)
                                    {
                                        case 4:
                                        {
                                            s32 res = (s32)(*(fp32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            s64 to_int =
                                                (s64)(*(fp64 *)(value));

                                            u32 res =
                                                (cast_to_int->is_signed)
                                                    ? ((to_int > 2147483647)
                                                           ? 2147483647LL
                                                           : ((to_int <
                                                               -2147483648LL)
                                                                  ? 2147483648
                                                                  : (u32)
                                                                        to_int))
                                                    : ((to_int > 4294967295)
                                                           ? 4294967295ULL
                                                           : ((to_int < 0)
                                                                  ? 0
                                                                  : (u32)
                                                                        to_int));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_POINTER:
                                {
                                    Type_Pointer *original_type_fp =
                                        (Type_Pointer *)original_type;

                                    u32 res = (u32)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                                case TYPE_FUNCTION:
                                {
                                    Type_Function *original_type_fp =
                                        (Type_Function *)original_type;

                                    u32 res = (u32)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                            }

                            break;
                        }
                        case 8:
                        {
                            switch (original_type->type_id)
                            {
                                case TYPE_INTEGER:
                                {
                                    Type_Integer *original_type_int =
                                        (Type_Integer *)original_type;

                                    switch (original_type_int->size)
                                    {
                                        case 1:
                                        {
                                            u64 res =
                                                (original_type_int->is_signed)
                                                    ? (s64)(*(s8 *)(value))
                                                    : (u64)(*(u8 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 2:
                                        {
                                            u64 res =
                                                (original_type_int->is_signed)
                                                    ? (s64)(*(s16 *)(value))
                                                    : (u64)(*(u16 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 4:
                                        {
                                            u64 res =
                                                (original_type_int->is_signed)
                                                    ? (s64)(*(s32 *)(value))
                                                    : (u64)(*(u32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            u64 res =
                                                (original_type_int->is_signed)
                                                    ? (s64)(*(s64 *)(value))
                                                    : (u64)(*(u64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_FLOAT:
                                {
                                    Type_Float *original_type_fp =
                                        (Type_Float *)original_type;

                                    switch (original_type_fp->size)
                                    {
                                        case 4:
                                        {
                                            s64 res = (s64)(*(fp32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            s64 res = (s64)(*(fp64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_int,
                                                &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_POINTER:
                                {
                                    Type_Pointer *original_type_fp =
                                        (Type_Pointer *)original_type;

                                    u64 res = (u64)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                                case TYPE_FUNCTION:
                                {
                                    Type_Function *original_type_fp =
                                        (Type_Function *)original_type;

                                    u64 res = (u64)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_int, &res);

                                    break;
                                }
                            }

                            break;
                        }
                    }

                    break;
                }
                case TYPE_FLOAT:
                {
                    Type_Float *cast_to_fp = (Type_Float *)cast_to;

                    switch (cast_to_fp->size)
                    {
                        case 4:
                        {
                            switch (original_type->type_id)
                            {
                                case TYPE_INTEGER:
                                {
                                    Type_Integer *original_type_int =
                                        (Type_Integer *)original_type;

                                    switch (original_type_int->size)
                                    {
                                        case 1:
                                        {
                                            fp32 res =
                                                (original_type_int->is_signed)
                                                    ? (fp32)(*(s8 *)(value))
                                                    : (fp32)(*(u8 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 2:
                                        {
                                            fp32 res =
                                                (original_type_int->is_signed)
                                                    ? (fp32)(*(s16 *)(value))
                                                    : (fp32)(*(u16 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 4:
                                        {
                                            fp32 res =
                                                (original_type_int->is_signed)
                                                    ? (fp32)(*(s32 *)(value))
                                                    : (fp32)(*(u32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            fp32 res =
                                                (original_type_int->is_signed)
                                                    ? (fp32)(*(s64 *)(value))
                                                    : (fp32)(*(u64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_FLOAT:
                                {
                                    Type_Float *original_type_fp =
                                        (Type_Float *)original_type;

                                    switch (original_type_fp->size)
                                    {
                                        case 4:
                                        {
                                            fp32 res = (fp32)(*(fp32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            fp32 res = (fp32)(*(fp64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_POINTER:
                                {
                                    Type_Pointer *original_type_fp =
                                        (Type_Pointer *)original_type;

                                    fp32 res = (fp32)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_fp, &res);

                                    break;
                                }
                                case TYPE_FUNCTION:
                                {
                                    Type_Function *original_type_fp =
                                        (Type_Function *)original_type;

                                    fp32 res = (fp32)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_fp, &res);

                                    break;
                                }
                            }

                            break;
                        }
                        case 8:
                        {
                            switch (original_type->type_id)
                            {
                                case TYPE_INTEGER:
                                {
                                    Type_Integer *original_type_int =
                                        (Type_Integer *)original_type;

                                    switch (original_type_int->size)
                                    {
                                        case 1:
                                        {
                                            fp64 res =
                                                (original_type_int->is_signed)
                                                    ? (fp64)(*(s8 *)(value))
                                                    : (fp64)(*(u8 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 2:
                                        {
                                            fp64 res =
                                                (original_type_int->is_signed)
                                                    ? (fp64)(*(s16 *)(value))
                                                    : (fp64)(*(u16 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 4:
                                        {
                                            fp64 res =
                                                (original_type_int->is_signed)
                                                    ? (fp64)(*(s32 *)(value))
                                                    : (fp64)(*(u32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            fp64 res =
                                                (original_type_int->is_signed)
                                                    ? (fp64)(*(s64 *)(value))
                                                    : (fp64)(*(u64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_FLOAT:
                                {
                                    Type_Float *original_type_fp =
                                        (Type_Float *)original_type;

                                    switch (original_type_fp->size)
                                    {
                                        case 4:
                                        {
                                            fp64 res = (fp64)(*(fp32 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                        case 8:
                                        {
                                            fp64 res = (fp64)(*(fp64 *)(value));

                                            Interpreter_stack_destroy_n(self,
                                                                        1);
                                            Interpreter_stack_push(
                                                self, (Type *)cast_to_fp, &res);
                                            break;
                                        }
                                    }

                                    break;
                                }
                                case TYPE_POINTER:
                                {
                                    Type_Pointer *original_type_fp =
                                        (Type_Pointer *)original_type;

                                    fp64 res = (fp64)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_fp, &res);

                                    break;
                                }
                                case TYPE_FUNCTION:
                                {
                                    Type_Function *original_type_fp =
                                        (Type_Function *)original_type;

                                    fp64 res = (fp64)(*(u64 *)(value));

                                    Interpreter_stack_destroy_n(self, 1);
                                    Interpreter_stack_push(
                                        self, (Type *)cast_to_fp, &res);

                                    break;
                                }
                            }

                            break;
                        }
                    }

                    break;
                }
                case TYPE_POINTER:
                case TYPE_FUNCTION:
                {
                    break;
                }
                default:
                {
                    puts("Invalid cast");
                    break;
                }
            }

            break;
        }
        case EXPRESSION_ASSIGN:
        {
            Expression_Assign *assign = (Expression_Assign *)expression;

            if (!Interpreter_evaluate(self, assign->rhs))
            {
                return 0;
            }

            if (assign->lhs->expression_id == EXPRESSION_REFERENCE)
            {
                Expression_Reference *ref = (Expression_Reference *)assign->lhs;

                Type *type_lhs, *type_rhs;
                void *value_lhs, *value_rhs;

                Interpreter_scope_get(self, ref->id, &type_lhs, &value_lhs);
                Interpreter_stack_get(self, &type_rhs, &value_rhs);

                if (!Type_equal(type_lhs, type_rhs))
                {
                    puts("Incompatible Types");
                    return 0;
                }

                Interpreter_scope_assign(self, ref->id, value_rhs);
            }
            else
            {
                puts("Left Hand Expression is not an LValue");
                return 0;
            }

            break;
        }
        case EXPRESSION_BINARY:
        {
            Expression_Binary *binary = (Expression_Binary *)expression;

            Interpreter_evaluate(self, binary->lhs);
            Interpreter_evaluate(self, binary->rhs);

            Type *type_lhs, *type_rhs;
            void *value_lhs, *value_rhs;

            Interpreter_stack_get2(self, &type_lhs, &value_lhs, &type_rhs,
                                   &value_rhs);

            if (!Type_equal(type_lhs, type_rhs))
            {
                puts("Incompatible Types");
                return 0;
            }

            Type *type = Type_dereference(type_lhs);

            switch (type->type_id)
            {
                case TYPE_INTEGER:
                {
                    Type_Integer *type_int = (Type_Integer *)type;

                    switch (type_int->size)
                    {
                        case 1:
                        {
                            u8 lhs = *((u8 *)value_lhs);
                            u8 rhs = *((u8 *)value_rhs);
                            u8 res;

                            switch (binary->op)
                            {
                                case '|':
                                    res = lhs | rhs;
                                    break;
                                case '^':
                                    res = lhs ^ rhs;
                                    break;
                                case '&':
                                    res = lhs & rhs;
                                    break;
                                case Tag_EQ:
                                    res = lhs == rhs;
                                    break;
                                case Tag_NE:
                                    res = lhs != rhs;
                                    break;
                                case '>':
                                    res = lhs > rhs;
                                    break;
                                case '<':
                                    res = lhs < rhs;
                                    break;
                                case Tag_GE:
                                    res = lhs >= rhs;
                                    break;
                                case Tag_LE:
                                    res = lhs <= rhs;
                                    break;
                                case Tag_LSHIFT:
                                    res = lhs << rhs;
                                    break;
                                case Tag_RSHIFT:
                                    res = lhs >> rhs;
                                    break;
                                case '+':
                                    res = lhs + rhs;
                                    break;
                                case '-':
                                    res = lhs - rhs;
                                    break;
                                case '*':
                                    res = lhs * rhs;
                                    break;
                                case '/':
                                    res = lhs / rhs;
                                    break;
                                case '%':
                                    res = lhs % rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 2);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 2:
                        {
                            u16 lhs = *((u16 *)value_lhs);
                            u16 rhs = *((u16 *)value_rhs);
                            u16 res;

                            switch (binary->op)
                            {
                                case '|':
                                    res = lhs | rhs;
                                    break;
                                case '^':
                                    res = lhs ^ rhs;
                                    break;
                                case '&':
                                    res = lhs & rhs;
                                    break;
                                case Tag_EQ:
                                    res = lhs == rhs;
                                    break;
                                case Tag_NE:
                                    res = lhs != rhs;
                                    break;
                                case '>':
                                    res = lhs > rhs;
                                    break;
                                case '<':
                                    res = lhs < rhs;
                                    break;
                                case Tag_GE:
                                    res = lhs >= rhs;
                                    break;
                                case Tag_LE:
                                    res = lhs <= rhs;
                                    break;
                                case Tag_LSHIFT:
                                    res = lhs << rhs;
                                    break;
                                case Tag_RSHIFT:
                                    res = lhs >> rhs;
                                    break;
                                case '+':
                                    res = lhs + rhs;
                                    break;
                                case '-':
                                    res = lhs - rhs;
                                    break;
                                case '*':
                                    res = lhs * rhs;
                                    break;
                                case '/':
                                    res = lhs / rhs;
                                    break;
                                case '%':
                                    res = lhs % rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 2);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 4:
                        {
                            u32 lhs = *((u32 *)value_lhs);
                            u32 rhs = *((u32 *)value_rhs);
                            u32 res;

                            switch (binary->op)
                            {
                                case '|':
                                    res = lhs | rhs;
                                    break;
                                case '^':
                                    res = lhs ^ rhs;
                                    break;
                                case '&':
                                    res = lhs & rhs;
                                    break;
                                case Tag_EQ:
                                    res = lhs == rhs;
                                    break;
                                case Tag_NE:
                                    res = lhs != rhs;
                                    break;
                                case '>':
                                    res = lhs > rhs;
                                    break;
                                case '<':
                                    res = lhs < rhs;
                                    break;
                                case Tag_GE:
                                    res = lhs >= rhs;
                                    break;
                                case Tag_LE:
                                    res = lhs <= rhs;
                                    break;
                                case Tag_LSHIFT:
                                    res = lhs << rhs;
                                    break;
                                case Tag_RSHIFT:
                                    res = lhs >> rhs;
                                    break;
                                case '+':
                                    res = lhs + rhs;
                                    break;
                                case '-':
                                    res = lhs - rhs;
                                    break;
                                case '*':
                                    res = lhs * rhs;
                                    break;
                                case '/':
                                    res = lhs / rhs;
                                    break;
                                case '%':
                                    res = lhs % rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 2);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 8:
                        {
                            u64 lhs = *((u64 *)value_lhs);
                            u64 rhs = *((u64 *)value_rhs);
                            u64 res;

                            switch (binary->op)
                            {
                                case '|':
                                    res = lhs | rhs;
                                    break;
                                case '^':
                                    res = lhs ^ rhs;
                                    break;
                                case '&':
                                    res = lhs & rhs;
                                    break;
                                case Tag_EQ:
                                    res = lhs == rhs;
                                    break;
                                case Tag_NE:
                                    res = lhs != rhs;
                                    break;
                                case '>':
                                    res = lhs > rhs;
                                    break;
                                case '<':
                                    res = lhs < rhs;
                                    break;
                                case Tag_GE:
                                    res = lhs >= rhs;
                                    break;
                                case Tag_LE:
                                    res = lhs <= rhs;
                                    break;
                                case Tag_LSHIFT:
                                    res = lhs << rhs;
                                    break;
                                case Tag_RSHIFT:
                                    res = lhs >> rhs;
                                    break;
                                case '+':
                                    res = lhs + rhs;
                                    break;
                                case '-':
                                    res = lhs - rhs;
                                    break;
                                case '*':
                                    res = lhs * rhs;
                                    break;
                                case '/':
                                    res = lhs / rhs;
                                    break;
                                case '%':
                                    res = lhs % rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 2);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                    }

                    break;
                }
                case TYPE_FLOAT:
                {
                    Type_Float *type_float = (Type_Float *)type;

                    switch (type_float->size)
                    {
                        case 4:
                        {
                            fp32 lhs = *((fp32 *)value_lhs);
                            fp32 rhs = *((fp32 *)value_rhs);
                            fp32 res;

                            switch (binary->op)
                            {
                                case Tag_EQ:
                                    res = lhs == rhs;
                                    break;
                                case Tag_NE:
                                    res = lhs != rhs;
                                    break;
                                case '>':
                                    res = lhs > rhs;
                                    break;
                                case '<':
                                    res = lhs < rhs;
                                    break;
                                case Tag_GE:
                                    res = lhs >= rhs;
                                    break;
                                case Tag_LE:
                                    res = lhs <= rhs;
                                    break;
                                case '+':
                                    res = lhs + rhs;
                                    break;
                                case '-':
                                    res = lhs - rhs;
                                    break;
                                case '*':
                                    res = lhs * rhs;
                                    break;
                                case '/':
                                    res = lhs / rhs;
                                    break;
                                case Tag_LSHIFT:
                                case Tag_RSHIFT:
                                case '%':
                                case '|':
                                case '^':
                                case '&':
                                    puts("Unsuported Operation");
                                    return 0;
                            }

                            Interpreter_stack_destroy_n(self, 2);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 8:
                        {
                            fp64 lhs = *((fp64 *)value_lhs);
                            fp64 rhs = *((fp64 *)value_rhs);
                            fp64 res;

                            switch (binary->op)
                            {
                                case Tag_EQ:
                                    res = lhs == rhs;
                                    break;
                                case Tag_NE:
                                    res = lhs != rhs;
                                    break;
                                case '>':
                                    res = lhs > rhs;
                                    break;
                                case '<':
                                    res = lhs < rhs;
                                    break;
                                case Tag_GE:
                                    res = lhs >= rhs;
                                    break;
                                case Tag_LE:
                                    res = lhs <= rhs;
                                    break;
                                case '+':
                                    res = lhs + rhs;
                                    break;
                                case '-':
                                    res = lhs - rhs;
                                    break;
                                case '*':
                                    res = lhs * rhs;
                                    break;
                                case '/':
                                    res = lhs / rhs;
                                    break;
                                case Tag_LSHIFT:
                                case Tag_RSHIFT:
                                case '%':
                                case '|':
                                case '^':
                                case '&':
                                    puts("Unsuported Operation");
                                    return 0;
                            }

                            Interpreter_stack_destroy_n(self, 2);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        default:
                        {
                            puts("Unsuported Operation");
                            return 0;
                        }
                    }

                    break;
                }
            }

            break;
        }
        case EXPRESSION_UNARY:
        {
            Expression_Unary *unary = (Expression_Unary *)expression;

            if (unary->op == '&')
            {
                return Interpreter_evaluate_get_address(self,
                                                        unary->expression);
            }

            Interpreter_evaluate(self, unary->expression);

            Type *type;
            void *value;

            Interpreter_stack_get(self, &type, &value);

            if (unary->op == '*')
            {
                if (type->type_id == TYPE_POINTER)
                {
                    Type_Pointer *type_pointer = (Type_Pointer *)type;
                    size_t size = Type_size(type_pointer->type);

                    void *value_dereferenced = malloc(size);

                    memcpy(value_dereferenced, value, size);

                    Interpreter_stack_destroy_n(self, 1);
                    Interpreter_stack_push(self, type_pointer->type,
                                           value_dereferenced);

                    free(value_dereferenced);
                    break;
                }
                else
                {
                    puts("Not a pointer");
                    return 0;
                }
            }

            type = Type_dereference(type);

            switch (type->type_id)
            {
                case TYPE_INTEGER:
                {
                    Type_Integer *type_int = (Type_Integer *)type;

                    switch (type_int->size)
                    {
                        case 1:
                        {
                            u8 rhs = *((u8 *)value);
                            u8 res;

                            switch (unary->op)
                            {
                                case '!':
                                    res = !rhs;
                                    break;
                                case '-':
                                    res = (u8)(-(s8)(rhs));
                                    break;
                                case '~':
                                    res = ~rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 1);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 2:
                        {
                            u16 rhs = *((u16 *)value);
                            u16 res;

                            switch (unary->op)
                            {
                                case '!':
                                    res = !rhs;
                                    break;
                                case '-':
                                    res = (u16)(-(s16)(rhs));
                                    break;
                                case '~':
                                    res = ~rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 1);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 4:
                        {
                            u32 rhs = *((u32 *)value);
                            u32 res;

                            switch (unary->op)
                            {
                                case '!':
                                    res = !rhs;
                                    break;
                                case '-':
                                    res = (u32)(-(s32)(rhs));
                                    break;
                                case '~':
                                    res = ~rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 1);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 8:
                        {
                            u64 rhs = *((u64 *)value);
                            u64 res;

                            switch (unary->op)
                            {
                                case '!':
                                    res = !rhs;
                                    break;
                                case '-':
                                    res = (u64)(-(s64)(rhs));
                                    break;
                                case '~':
                                    res = ~rhs;
                                    break;
                            }

                            Interpreter_stack_destroy_n(self, 1);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                    }

                    break;
                }
                case TYPE_FLOAT:
                {
                    Type_Float *type_fp = (Type_Float *)type;

                    switch (type_fp->size)
                    {
                        case 4:
                        {
                            fp32 rhs = *((fp32 *)value);
                            fp32 res;

                            switch (unary->op)
                            {
                                case '!':
                                    res = !rhs;
                                    break;
                                case '-':
                                    res = -rhs;
                                    break;
                                default:
                                    puts("Unsupported operation");
                                    return 0;
                            }

                            Interpreter_stack_destroy_n(self, 1);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                        case 8:
                        {
                            fp64 rhs = *((fp64 *)value);
                            fp64 res;

                            switch (unary->op)
                            {
                                case '!':
                                    res = !rhs;
                                    break;
                                case '-':
                                    res = -rhs;
                                    break;
                                default:
                                    puts("Unsupported operation");
                                    return 0;
                            }

                            Interpreter_stack_destroy_n(self, 1);
                            Interpreter_stack_push(self, type, &res);
                            break;
                        }
                    }

                    break;
                }
                default:
                {
                    puts("Unsuported Operation");
                    return 0;
                }
            }

            break;
        }
        case EXPRESSION_INTEGER_LITERAL:
        {
            Expression_Integer_Literal *lit =
                (Expression_Integer_Literal *)expression;

            u64 value = strtoull(String_begin(lit->value), NULL, 10);
            Type *type = *MAP_GET(Type *, self->types, id_s64);

            Interpreter_stack_push(self, type, &value);
            break;
        }
        case EXPRESSION_REAL_LITERAL:
        {
            Expression_Real_Literal *lit =
                (Expression_Real_Literal *)expression;

            fp64 value = strtod(String_begin(lit->value), NULL);
            Type *type = *MAP_GET(Type *, self->types, id_fp64);

            Interpreter_stack_push(self, type, &value);
            break;
        }
        case EXPRESSION_REFERENCE:
        {
            Expression_Reference *ref = (Expression_Reference *)expression;

            Type *type;
            void *value;

            Interpreter_scope_get(self, ref->id, &type, &value);
            Interpreter_stack_push(self, type, value);
            break;
        }
    }

    return 1;
}
