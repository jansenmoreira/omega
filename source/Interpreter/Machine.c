/*
#include <Interpreter/Machine.h>

Scope* Scope_Create(Scope* parent)
{
    Scope* self = (Scope*)malloc(sizeof(Scope));
    self->parent = parent;
    self->table = MAP_CREATE(size_t);
    self->values = STACK_CREATE(u8);
    return self;
}

void Scope_destroy(Scope* self)
{
    u8* at = self->values.buffer;

    for (size_t i = 0; i < self->table.size; i++)
    {
        Type** ptr = (Type**)(at);
        at += sizeof(Type*) + Type_size(*ptr);
        Type_destroy(*ptr);
    }

    MAP_DESTROY(size_t, self->table);
    STACK_DESTROY(u8, self->values);
}

void* Scope_get(Scope* self, String id)
{
    do
    {
        size_t* index = MAP_GET(size_t, self->table, id);

        if (!index)
        {
            continue;
        }

        return self->values.buffer + *index;
    } while (self->parent);

    return NULL;
}

Machine Machine_create()
{
    Machine self;
    self.global = Scope_Create(NULL);
    self.actual = self.global;
    self.stack = STACK_CREATE(u8);
    self.types = MAP_CREATE(Type*);

    String id_u8 = String_new("u8", 2);
    Type* t_u8 = (Type*)Type_Integer_create(1, 0);
    MAP_SET(Type*, self.types, id_u8, t_u8);

    String id_u16 = String_new("u16", 3);
    Type* t_u16 = (Type*)Type_Integer_create(2, 0);
    MAP_SET(Type*, self.types, id_u16, t_u16);

    String id_u32 = String_new("u32", 3);
    Type* t_u32 = (Type*)Type_Integer_create(4, 0);
    MAP_SET(Type*, self.types, id_u32, t_u32);

    String id_u64 = String_new("u64", 3);
    Type* t_u64 = (Type*)Type_Integer_create(8, 0);
    MAP_SET(Type*, self.types, id_u64, t_u64);

    String id_s8 = String_new("s8", 2);
    Type* t_s8 = (Type*)Type_Integer_create(1, 1);
    MAP_SET(Type*, self.types, id_s8, t_s8);

    String id_s16 = String_new("s16", 3);
    Type* t_s16 = (Type*)Type_Integer_create(2, 1);
    MAP_SET(Type*, self.types, id_s16, t_s16);

    String id_s32 = String_new("s32", 3);
    Type* t_s32 = (Type*)Type_Integer_create(4, 1);
    MAP_SET(Type*, self.types, id_s32, t_s32);

    String id_s64 = String_new("s64", 3);
    Type* t_s64 = (Type*)Type_Integer_create(8, 1);
    MAP_SET(Type*, self.types, id_s64, t_s64);

    String id_fp32 = String_new("fp32", 4);
    Type* t_fp32 = (Type*)Type_Float_create(4);
    MAP_SET(Type*, self.types, id_fp32, t_fp32);

    String id_fp64 = String_new("fp64", 4);
    Type* t_fp64 = (Type*)Type_Float_create(8);
    MAP_SET(Type*, self.types, id_fp64, t_fp64);

    return self;
}

void Machine_destroy(Machine* self)
{
    Scope* scope = self->actual;
    do
    {
        Scope_destroy(scope);
        scope = scope->parent;
    } while (scope);

    STACK_DESTROY(u8, self->stack);
    MAP_DESTROY(Type*, self->types);
}

void Machine_scope_push(Machine* self)
{
    self->actual = Scope_Create(self->actual->parent);
}

void Machine_scope_pop(Machine* self)
{
    Scope* scope = self->actual;
    self->actual = self->actual->parent;
    Scope_destroy(scope);
}

void Machine_scope_get(Machine* self, String id, Type** type, void** value)
{
    Scope* scope = self->actual;

    while (scope)
    {
        size_t* index = MAP_GET(size_t, scope->table, id);

        if (index)
        {
            *type = *((Type**)(self->global->values.buffer + *index));

            *value =
                ((void*)(self->global->values.buffer + *index + sizeof(Type*)));

            return;
        }

        scope = self->actual->parent;
    }

    type = NULL;
    value = NULL;
}

void Machine_scope_add(Machine* self, String id, Type* type, void* value)
{
    size_t index = self->actual->values.size;

    size_t amount = sizeof(Type*) + Type_size(type);
    STACK_GROW(u8, self->actual->values, amount);

    MAP_SET(size_t, self->global->table, id, index);

    if (value)
    {
        void* value_dst =
            ((void*)(self->global->values.buffer + index + sizeof(Type*)));

        memcpy(value_dst, value, Type_size(type));
    }
}

void Machine_scope_assign(Machine* self, String id, void* value)
{
    Type* type = NULL;
    void* value_dst = NULL;

    Machine_scope_get(self, id, &type, &value_dst);

    if (type && value_dst)
    {
        memcpy(value_dst, value, Type_size(type));
    }
}

void Machine_stack_get(Machine* self, Type** type, void** value)
{
    *type = *((Type**)(self->stack.buffer + self->stack.size - sizeof(Type*)));

    *value = (void*)(self->stack.buffer + self->stack.size - sizeof(Type*) -
                     Type_size(*type));
}

void Machine_stack_get2(Machine* self, Type** type_lhs, void** value_lhs,
                        Type** type_rhs, void** value_rhs)
{
    *type_rhs =
        *((Type**)(self->stack.buffer + self->stack.size - sizeof(Type*)));

    *value_rhs = (void*)(self->stack.buffer + self->stack.size - sizeof(Type*) -
                         Type_size(*type_rhs));

    *type_lhs =
        *((Type**)(self->stack.buffer + self->stack.size - sizeof(Type*) -
                   Type_size(*type_rhs) - sizeof(Type*)));

    *value_lhs =
        (void*)(self->stack.buffer + self->stack.size - sizeof(Type*) -
                Type_size(*type_rhs) - sizeof(Type*) - Type_size(*type_lhs));
}

void Machine_stack_destroy_n(Machine* self, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        Type* type;
        void* value;

        Machine_stack_get(self, &type, &value);

        size_t amount = sizeof(Type*) + Type_size(type);
        Type_destroy(type);

        STACK_SHRINK(u8, self->stack, amount);
    }
}

void Machine_stack_push(Machine* self, Type* type, void* value)
{
    size_t stack_size = self->stack.size;

    size_t amount = Type_size(type) + sizeof(Type*);
    STACK_GROW(u8, self->stack, amount);

    memcpy(self->stack.buffer + stack_size, value, Type_size(type));
    memcpy(self->stack.buffer + stack_size + Type_size(type), &type,
           sizeof(Type*));
}

void Machine_print_top(Machine* self)
{
    Type* type;
    void* value;

    Machine_stack_get(self, &type, &value);

    if (type && value)
    {
        Type_print(type);
        printf(" (%" PRIu64 ") => ", Type_size(type));
        Type_print_value(type, value);
        puts("");

        Machine_stack_destroy_n(self, 1);
    }
}

Boolean Machine_evaluate_cast(Machine* self, Expression_Cast* cast)
{
    if (!Machine_evaluate(self, cast->expression))
    {
        return False;
    }

    Type* type;
    void* value;

    Machine_stack_get(self, &type, &value);

    switch (cast->type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer* type_destination =
                (Type_Integer*)Type_Copy(cast->type);

            switch (type_destination->size)
            {
                case 1:
                {
                    switch (type->type_id)
                    {
                        case TYPE_INTEGER:
                        {
                            Type_Integer* type_integer = (Type_Integer*)type;

                            switch (type_integer->size)
                            {
                                case 1:
                                {
                                    u8 result = (type_integer->is_signed)
                                                    ? (s8)(*(s8*)(value))
                                                    : (u8)(*(u8*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 2:
                                {
                                    u8 result = (type_integer->is_signed)
                                                    ? (s8)(*(s16*)(value))
                                                    : (u8)(*(u16*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 4:
                                {
                                    u8 result = (type_integer->is_signed)
                                                    ? (s8)(*(s32*)(value))
                                                    : (u8)(*(u32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    u8 result = (type_integer->is_signed)
                                                    ? (s8)(*(s64*)(value))
                                                    : (u8)(*(u64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_FLOAT:
                        {
                            Type_Float* type_float = (Type_Float*)type;

                            switch (type_float->size)
                            {
                                case 4:
                                {
                                    s64 to_int = (s64)(*(fp32*)(value));

                                    u8 result =
                                        (type_destination->is_signed)
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

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    s64 to_int = (s64)(*(fp64*)(value));

                                    u8 result =
                                        (type_destination->is_signed)
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

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_POINTER:
                        case TYPE_FUNCTION:
                        {
                            u8 result = (u8)(*(u64*)(value));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, (Type*)type_destination,
                                               &result);

                            return True;
                        }
                    }

                    break;
                }
                case 2:
                {
                    switch (type->type_id)
                    {
                        case TYPE_INTEGER:
                        {
                            Type_Integer* type_integer = (Type_Integer*)type;

                            switch (type_integer->size)
                            {
                                case 1:
                                {
                                    u16 result = (type_integer->is_signed)
                                                     ? (s16)(*(s8*)(value))
                                                     : (u16)(*(u8*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 2:
                                {
                                    u16 result = (type_integer->is_signed)
                                                     ? (s16)(*(s16*)(value))
                                                     : (u16)(*(u16*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 4:
                                {
                                    u16 result = (type_integer->is_signed)
                                                     ? (s16)(*(s32*)(value))
                                                     : (u16)(*(u32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    u16 result = (type_integer->is_signed)
                                                     ? (s16)(*(s64*)(value))
                                                     : (u16)(*(u64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_FLOAT:
                        {
                            Type_Float* type_float = (Type_Float*)type;

                            switch (type_float->size)
                            {
                                case 4:
                                {
                                    s32 to_int = (s32)(*(fp32*)(value));

                                    u16 result =
                                        (type_destination->is_signed)
                                            ? ((to_int > 127)
                                                   ? 127
                                                   : ((to_int < -128)
                                                          ? 128
                                                          : (u16)to_int))
                                            : ((to_int > 255)
                                                   ? 255
                                                   : ((to_int < 0)
                                                          ? 0
                                                          : (u16)to_int));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    s64 to_int = (s64)(*(fp64*)(value));

                                    u16 result =
                                        (type_destination->is_signed)
                                            ? (to_int > 32767
                                                   ? 32767
                                                   : to_int < -32768
                                                         ? -32768
                                                         : (u16)to_int)
                                            : (to_int > 65535
                                                   ? 65535
                                                   : to_int < 0 ? 0
                                                                : (u16)to_int);

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_POINTER:
                        case TYPE_FUNCTION:
                        {
                            u16 result = (u16)(*(u64*)(value));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, (Type*)type_destination,
                                               &result);

                            return True;
                        }
                    }

                    break;
                }
                case 4:
                {
                    switch (type->type_id)
                    {
                        case TYPE_INTEGER:
                        {
                            Type_Integer* type_integer = (Type_Integer*)type;

                            switch (type_integer->size)
                            {
                                case 1:
                                {
                                    u32 result = (type_integer->is_signed)
                                                     ? (s32)(*(s8*)(value))
                                                     : (u32)(*(u8*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 2:
                                {
                                    u32 result = (type_integer->is_signed)
                                                     ? (s32)(*(s16*)(value))
                                                     : (u32)(*(u16*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 4:
                                {
                                    u32 result = (type_integer->is_signed)
                                                     ? (s32)(*(s32*)(value))
                                                     : (u32)(*(u32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    u32 result = (type_integer->is_signed)
                                                     ? (s32)(*(s64*)(value))
                                                     : (u32)(*(u64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_FLOAT:
                        {
                            Type_Float* type_float = (Type_Float*)type;

                            switch (type_float->size)
                            {
                                case 4:
                                {
                                    s32 result = (s32)(*(fp32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    s64 to_int = (s64)(*(fp64*)(value));

                                    u32 result =
                                        (type_destination->is_signed)
                                            ? ((to_int > 2147483647)
                                                   ? 2147483647LL
                                                   : ((to_int < -2147483648LL)
                                                          ? 2147483648
                                                          : (u32)to_int))
                                            : ((to_int > 4294967295)
                                                   ? 4294967295ULL
                                                   : ((to_int < 0)
                                                          ? 0
                                                          : (u32)to_int));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_POINTER:
                        case TYPE_FUNCTION:
                        {
                            u32 result = (u32)(*(u64*)(value));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, (Type*)type_destination,
                                               &result);

                            return True;
                        }
                    }

                    break;
                }
                case 8:
                {
                    switch (type->type_id)
                    {
                        case TYPE_INTEGER:
                        {
                            Type_Integer* type_integer = (Type_Integer*)type;

                            switch (type_integer->size)
                            {
                                case 1:
                                {
                                    u64 result = (type_integer->is_signed)
                                                     ? (s64)(*(s8*)(value))
                                                     : (u64)(*(u8*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 2:
                                {
                                    u64 result = (type_integer->is_signed)
                                                     ? (s64)(*(s16*)(value))
                                                     : (u64)(*(u16*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 4:
                                {
                                    u64 result = (type_integer->is_signed)
                                                     ? (s64)(*(s32*)(value))
                                                     : (u64)(*(u32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    u64 result = (type_integer->is_signed)
                                                     ? (s64)(*(s64*)(value))
                                                     : (u64)(*(u64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_FLOAT:
                        {
                            Type_Float* type_float = (Type_Float*)type;

                            switch (type_float->size)
                            {
                                case 4:
                                {
                                    s64 result = (s64)(*(fp32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    s64 result = (s64)(*(fp64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_POINTER:
                        case TYPE_FUNCTION:
                        {
                            u64 result = (u64)(*(u64*)(value));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, (Type*)type_destination,
                                               &result);

                            return True;
                        }
                    }

                    break;
                }
            }

            break;
        }
        case TYPE_FLOAT:
        {
            Type_Float* type_destination = (Type_Float*)Type_Copy(cast->type);

            switch (type_destination->size)
            {
                case 4:
                {
                    switch (type->type_id)
                    {
                        case TYPE_INTEGER:
                        {
                            Type_Integer* type_integer = (Type_Integer*)type;

                            switch (type_integer->size)
                            {
                                case 1:
                                {
                                    fp32 result = (type_integer->is_signed)
                                                      ? (fp32)(*(s8*)(value))
                                                      : (fp32)(*(u8*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 2:
                                {
                                    fp32 result = (type_integer->is_signed)
                                                      ? (fp32)(*(s16*)(value))
                                                      : (fp32)(*(u16*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 4:
                                {
                                    fp32 result = (type_integer->is_signed)
                                                      ? (fp32)(*(s32*)(value))
                                                      : (fp32)(*(u32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    fp32 result = (type_integer->is_signed)
                                                      ? (fp32)(*(s64*)(value))
                                                      : (fp32)(*(u64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_FLOAT:
                        {
                            Type_Float* type_float = (Type_Float*)type;

                            switch (type_float->size)
                            {
                                case 4:
                                {
                                    fp32 result = (fp32)(*(fp32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    fp32 result = (fp32)(*(fp64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_POINTER:
                        case TYPE_FUNCTION:
                        {
                            fp32 result = (fp32)(*(u64*)(value));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, (Type*)type_destination,
                                               &result);

                            return True;
                        }
                    }

                    break;
                }
                case 8:
                {
                    switch (type->type_id)
                    {
                        case TYPE_INTEGER:
                        {
                            Type_Integer* type_integer = (Type_Integer*)type;

                            switch (type_integer->size)
                            {
                                case 1:
                                {
                                    fp64 result = (type_integer->is_signed)
                                                      ? (fp64)(*(s8*)(value))
                                                      : (fp64)(*(u8*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 2:
                                {
                                    fp64 result = (type_integer->is_signed)
                                                      ? (fp64)(*(s16*)(value))
                                                      : (fp64)(*(u16*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 4:
                                {
                                    fp64 result = (type_integer->is_signed)
                                                      ? (fp64)(*(s32*)(value))
                                                      : (fp64)(*(u32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    fp64 result = (type_integer->is_signed)
                                                      ? (fp64)(*(s64*)(value))
                                                      : (fp64)(*(u64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_FLOAT:
                        {
                            Type_Float* type_float = (Type_Float*)type;

                            switch (type_float->size)
                            {
                                case 4:
                                {
                                    fp64 result = (fp64)(*(fp32*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                                case 8:
                                {
                                    fp64 result = (fp64)(*(fp64*)(value));

                                    Machine_stack_destroy_n(self, 1);
                                    Machine_stack_push(
                                        self, (Type*)type_destination, &result);

                                    return True;
                                }
                            }

                            break;
                        }
                        case TYPE_POINTER:
                        case TYPE_FUNCTION:
                        {
                            fp64 result = (fp64)(*(u64*)(value));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, (Type*)type_destination,
                                               &result);

                            return True;
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
            switch (type->type_id)
            {
                case TYPE_INTEGER:
                {
                    Type_Integer* type_integer = (Type_Integer*)type;

                    switch (type_integer->size)
                    {
                        case 1:
                        {
                            void* result = (void*)(u64)((*(u8*)(value)));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, cast->type, &result);

                            return True;
                        }
                        case 2:
                        {
                            void* result = (void*)(u64)((*(u16*)(value)));

                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, cast->type, &result);

                            return True;
                        }
                        case 4:
                        {
                            void* result = (void*)(u64)((*(u32*)(value)));

                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, cast->type, &result);

                            return True;
                        }
                        case 8:
                        {
                            void* result = (void*)(*(u64*)(value));
                            Machine_stack_destroy_n(self, 1);
                            Machine_stack_push(self, cast->type, &result);

                            return True;
                        }
                    }

                    break;
                }
                case TYPE_POINTER:
                case TYPE_FUNCTION:
                {
                    void* result = (*(void**)(value));
                    Machine_stack_destroy_n(self, 1);
                    Machine_stack_push(self, cast->type, &result);

                    return True;
                }
            }

            break;
        }
    }

    return False;
}

Boolean Machine_evaluate_assign(Machine* self, Expression_Assign* assign)
{
    if (!Machine_evaluate(self, assign->rhs))
    {
        return False;
    }

    if (assign->lhs->expression_id == EXPRESSION_REFERENCE)
    {
        Expression_Reference* reference = (Expression_Reference*)assign->lhs;

        Type *type_lhs, *type_rhs;
        void *value_lhs, *value_rhs;

        Machine_scope_get(self, reference->id, &type_lhs, &value_lhs);
        Machine_stack_get(self, &type_rhs, &value_rhs);

        if (!Type_equal(type_lhs, type_rhs))
        {
            return False;
        }

        Machine_scope_assign(self, reference->id, value_rhs);
    }
    else
    {
        return False;
    }
}

Boolean Machine_evaluate_binary(Machine* self, Expression_Binary* binary)
{
    if (!Machine_evaluate(self, binary->lhs) ||
        !Machine_evaluate(self, binary->rhs))
    {
        return False;
    }

    Type *type_lhs, *type_rhs;
    void *value_lhs, *value_rhs;

    Machine_stack_get2(self, &type_lhs, &value_lhs, &type_rhs, &value_rhs);

    if (!Type_equal(type_lhs, type_rhs))
    {
        return False;
    }

    Type* type = Type_Copy(type_lhs);

    switch (type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer* type_integer = (Type_Integer*)type;

            switch (type_integer->size)
            {
                case 1:
                {
                    u8 lhs = *((u8*)value_lhs);
                    u8 rhs = *((u8*)value_rhs);
                    u8 result;

                    switch (binary->op)
                    {
                        case '|':
                            result = lhs | rhs;
                            break;
                        case '^':
                            result = lhs ^ rhs;
                            break;
                        case '&':
                            result = lhs & rhs;
                            break;
                        case Tag_EQ:
                            result = lhs == rhs;
                            break;
                        case Tag_NE:
                            result = lhs != rhs;
                            break;
                        case '>':
                            result = lhs > rhs;
                            break;
                        case '<':
                            result = lhs < rhs;
                            break;
                        case Tag_GE:
                            result = lhs >= rhs;
                            break;
                        case Tag_LE:
                            result = lhs <= rhs;
                            break;
                        case Tag_LSHIFT:
                            result = lhs << rhs;
                            break;
                        case Tag_RSHIFT:
                            result = lhs >> rhs;
                            break;
                        case '+':
                            result = lhs + rhs;
                            break;
                        case '-':
                            result = lhs - rhs;
                            break;
                        case '*':
                            result = lhs * rhs;
                            break;
                        case '/':
                            result = lhs / rhs;
                            break;
                        case '%':
                            result = lhs % rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 2);
                    Machine_stack_push(self, type, &result);
                    break;
                }
                case 2:
                {
                    u16 lhs = *((u16*)value_lhs);
                    u16 rhs = *((u16*)value_rhs);
                    u16 result;

                    switch (binary->op)
                    {
                        case '|':
                            result = lhs | rhs;
                            break;
                        case '^':
                            result = lhs ^ rhs;
                            break;
                        case '&':
                            result = lhs & rhs;
                            break;
                        case Tag_EQ:
                            result = lhs == rhs;
                            break;
                        case Tag_NE:
                            result = lhs != rhs;
                            break;
                        case '>':
                            result = lhs > rhs;
                            break;
                        case '<':
                            result = lhs < rhs;
                            break;
                        case Tag_GE:
                            result = lhs >= rhs;
                            break;
                        case Tag_LE:
                            result = lhs <= rhs;
                            break;
                        case Tag_LSHIFT:
                            result = lhs << rhs;
                            break;
                        case Tag_RSHIFT:
                            result = lhs >> rhs;
                            break;
                        case '+':
                            result = lhs + rhs;
                            break;
                        case '-':
                            result = lhs - rhs;
                            break;
                        case '*':
                            result = lhs * rhs;
                            break;
                        case '/':
                            result = lhs / rhs;
                            break;
                        case '%':
                            result = lhs % rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 2);
                    Machine_stack_push(self, type, &result);
                    break;
                }
                case 4:
                {
                    u32 lhs = *((u32*)value_lhs);
                    u32 rhs = *((u32*)value_rhs);
                    u32 result;

                    switch (binary->op)
                    {
                        case '|':
                            result = lhs | rhs;
                            break;
                        case '^':
                            result = lhs ^ rhs;
                            break;
                        case '&':
                            result = lhs & rhs;
                            break;
                        case Tag_EQ:
                            result = lhs == rhs;
                            break;
                        case Tag_NE:
                            result = lhs != rhs;
                            break;
                        case '>':
                            result = lhs > rhs;
                            break;
                        case '<':
                            result = lhs < rhs;
                            break;
                        case Tag_GE:
                            result = lhs >= rhs;
                            break;
                        case Tag_LE:
                            result = lhs <= rhs;
                            break;
                        case Tag_LSHIFT:
                            result = lhs << rhs;
                            break;
                        case Tag_RSHIFT:
                            result = lhs >> rhs;
                            break;
                        case '+':
                            result = lhs + rhs;
                            break;
                        case '-':
                            result = lhs - rhs;
                            break;
                        case '*':
                            result = lhs * rhs;
                            break;
                        case '/':
                            result = lhs / rhs;
                            break;
                        case '%':
                            result = lhs % rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 2);
                    Machine_stack_push(self, type, &result);
                    break;
                }
                case 8:
                {
                    u64 lhs = *((u64*)value_lhs);
                    u64 rhs = *((u64*)value_rhs);
                    u64 result;

                    switch (binary->op)
                    {
                        case '|':
                            result = lhs | rhs;
                            break;
                        case '^':
                            result = lhs ^ rhs;
                            break;
                        case '&':
                            result = lhs & rhs;
                            break;
                        case Tag_EQ:
                            result = lhs == rhs;
                            break;
                        case Tag_NE:
                            result = lhs != rhs;
                            break;
                        case '>':
                            result = lhs > rhs;
                            break;
                        case '<':
                            result = lhs < rhs;
                            break;
                        case Tag_GE:
                            result = lhs >= rhs;
                            break;
                        case Tag_LE:
                            result = lhs <= rhs;
                            break;
                        case Tag_LSHIFT:
                            result = lhs << rhs;
                            break;
                        case Tag_RSHIFT:
                            result = lhs >> rhs;
                            break;
                        case '+':
                            result = lhs + rhs;
                            break;
                        case '-':
                            result = lhs - rhs;
                            break;
                        case '*':
                            result = lhs * rhs;
                            break;
                        case '/':
                            result = lhs / rhs;
                            break;
                        case '%':
                            result = lhs % rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 2);
                    Machine_stack_push(self, type, &result);
                    break;
                }
            }

            break;
        }
        case TYPE_FLOAT:
        {
            Type_Float* type_float = (Type_Float*)type;

            switch (type_float->size)
            {
                case 4:
                {
                    fp32 lhs = *((fp32*)value_lhs);
                    fp32 rhs = *((fp32*)value_rhs);
                    fp32 result;

                    switch (binary->op)
                    {
                        case Tag_EQ:
                            result = lhs == rhs;
                            break;
                        case Tag_NE:
                            result = lhs != rhs;
                            break;
                        case '>':
                            result = lhs > rhs;
                            break;
                        case '<':
                            result = lhs < rhs;
                            break;
                        case Tag_GE:
                            result = lhs >= rhs;
                            break;
                        case Tag_LE:
                            result = lhs <= rhs;
                            break;
                        case '+':
                            result = lhs + rhs;
                            break;
                        case '-':
                            result = lhs - rhs;
                            break;
                        case '*':
                            result = lhs * rhs;
                            break;
                        case '/':
                            result = lhs / rhs;
                            break;
                        case Tag_LSHIFT:
                        case Tag_RSHIFT:
                        case '%':
                        case '|':
                        case '^':
                        case '&':
                            return False;
                    }

                    Machine_stack_destroy_n(self, 2);
                    Machine_stack_push(self, type, &result);
                    break;
                }
                case 8:
                {
                    fp64 lhs = *((fp64*)value_lhs);
                    fp64 rhs = *((fp64*)value_rhs);
                    fp64 result;

                    switch (binary->op)
                    {
                        case Tag_EQ:
                            result = lhs == rhs;
                            break;
                        case Tag_NE:
                            result = lhs != rhs;
                            break;
                        case '>':
                            result = lhs > rhs;
                            break;
                        case '<':
                            result = lhs < rhs;
                            break;
                        case Tag_GE:
                            result = lhs >= rhs;
                            break;
                        case Tag_LE:
                            result = lhs <= rhs;
                            break;
                        case '+':
                            result = lhs + rhs;
                            break;
                        case '-':
                            result = lhs - rhs;
                            break;
                        case '*':
                            result = lhs * rhs;
                            break;
                        case '/':
                            result = lhs / rhs;
                            break;
                        case Tag_LSHIFT:
                        case Tag_RSHIFT:
                        case '%':
                        case '|':
                        case '^':
                        case '&':
                            return False;
                    }

                    Machine_stack_destroy_n(self, 2);
                    Machine_stack_push(self, type, &result);
                    break;
                }
                default:
                {
                    return False;
                }
            }

            break;
        }
    }

    return True;
}

Boolean Machine_evaluate_unary(Machine* self, Expression_Unary* unary)
{
    if (unary->op == '&' &&
        unary->expression->expression_id == EXPRESSION_REFERENCE)
    {
        Expression_Reference* reference =
            (Expression_Reference*)unary->expression;

        Type* type;
        void* value;

        Machine_scope_get(self, reference->id, &type, &value);

        Type_Pointer* pointer = Type_Pointer_create(Type_Copy(type));

        Machine_stack_push(self, (Type*)pointer, &value);

        return True;
    }

    Machine_evaluate(self, unary->expression);

    Type* type;
    void* value;

    Machine_stack_get(self, &type, &value);

    if (unary->op == '*')
    {
        if (type->type_id == TYPE_POINTER)
        {
            Type_Pointer* type_pointer = (Type_Pointer*)type;
            size_t size = Type_size(type_pointer->type);

            void* value_dereferenced = malloc(size);

            memcpy(value_dereferenced, *(void **)(value), size);

            Type* type_dereferenced = Type_Copy(type_pointer->type);

            Machine_stack_destroy_n(self, 1);
            Machine_stack_push(self, type_dereferenced, value_dereferenced);

            free(value_dereferenced);

            return True;
        }

        return False;
    }

    switch (type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer* type_integer = (Type_Integer*)type;

            switch (type_integer->size)
            {
                case 1:
                {
                    u8 rhs = *((u8*)value);
                    u8 result;

                    switch (unary->op)
                    {
                        case '!':
                            result = !rhs;
                            break;
                        case '-':
                            result = (u8)(-(s8)(rhs));
                            break;
                        case '~':
                            result = ~rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 1);
                    Machine_stack_push(self, Type_Copy(type), &result);

                    break;
                }
                case 2:
                {
                    u16 rhs = *((u16*)value);
                    u16 result;

                    switch (unary->op)
                    {
                        case '!':
                            result = !rhs;
                            break;
                        case '-':
                            result = (u16)(-(s16)(rhs));
                            break;
                        case '~':
                            result = ~rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 1);
                    Machine_stack_push(self, Type_Copy(type), &result);

                    break;
                }
                case 4:
                {
                    u32 rhs = *((u32*)value);
                    u32 result;

                    switch (unary->op)
                    {
                        case '!':
                            result = !rhs;
                            break;
                        case '-':
                            result = (u32)(-(s32)(rhs));
                            break;
                        case '~':
                            result = ~rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 1);
                    Machine_stack_push(self, Type_Copy(type), &result);

                    break;
                }
                case 8:
                {
                    u64 rhs = *((u64*)value);
                    u64 result;

                    switch (unary->op)
                    {
                        case '!':
                            result = !rhs;
                            break;
                        case '-':
                            result = (u64)(-(s64)(rhs));
                            break;
                        case '~':
                            result = ~rhs;
                            break;
                    }

                    Machine_stack_destroy_n(self, 1);
                    Machine_stack_push(self, Type_Copy(type), &result);

                    break;
                }
            }

            break;
        }
        case TYPE_FLOAT:
        {
            Type_Float* type_float = (Type_Float*)type;

            switch (type_float->size)
            {
                case 4:
                {
                    fp32 rhs = *((fp32*)value);
                    fp32 result;

                    switch (unary->op)
                    {
                        case '!':
                            result = !rhs;
                            break;
                        case '-':
                            result = -rhs;
                            break;
                        default:
                            return False;
                    }

                    Machine_stack_destroy_n(self, 1);
                    Machine_stack_push(self, type, &result);

                    break;
                }
                case 8:
                {
                    fp64 rhs = *((fp64*)value);
                    fp64 result;

                    switch (unary->op)
                    {
                        case '!':
                            result = !rhs;
                            break;
                        case '-':
                            result = -rhs;
                            break;
                        default:
                            return False;
                    }

                    Machine_stack_destroy_n(self, 1);
                    Machine_stack_push(self, type, &result);

                    break;
                }
            }

            break;
        }
        default:
        {
            return False;
        }
    }

    return True;
}

Boolean Machine_evaluate(Machine* self, Expression* expression)
{
    switch (expression->expression_id)
    {
        case EXPRESSION_CAST:
        {
            return Machine_evaluate_cast(self, (Expression_Cast*)expression);
        }
        case EXPRESSION_ASSIGN:
        {
            return Machine_evaluate_assign(self,
                                           (Expression_Assign*)expression);
        }
        case EXPRESSION_BINARY:
        {
            return Machine_evaluate_binary(self,
                                           (Expression_Binary*)expression);
        }
        case EXPRESSION_UNARY:
        {
            return Machine_evaluate_unary(self, (Expression_Unary*)expression);
        }
        case EXPRESSION_STRING_LITERAL:
        {
            Expression_String_Literal* lit =
                (Expression_String_Literal*)expression;

            u8* value = String_begin(lit->value);
            Type* type = Type_Integer_create(1, False);
            Type* type_ptr = Type_Pointer_create(type);

            Machine_stack_push(self, type_ptr, &value);

            return True;
        }
        case EXPRESSION_INTEGER_LITERAL:
        {
            Expression_Integer_Literal* lit =
                (Expression_Integer_Literal*)expression;

            u64 value = strtoull(String_begin(lit->value), NULL, 10);
            Type* type = Type_Integer_create(8, True);

            Machine_stack_push(self, type, &value);

            return True;
        }
        case EXPRESSION_REAL_LITERAL:
        {
            Expression_Real_Literal* lit = (Expression_Real_Literal*)expression;

            fp64 value = strtod(String_begin(lit->value), NULL);
            Type* type = Type_Float_create(8);

            Machine_stack_push(self, type, &value);

            return True;
        }
        case EXPRESSION_REFERENCE:
        {
            Expression_Reference* reference = (Expression_Reference*)expression;

            Type* type;
            void* value;

            Machine_scope_get(self, reference->id, &type, &value);
            Machine_stack_push(self, Type_Copy(type), value);

            return True;
        }
        default:
        {
            return False;
        }
    }
}
*/
