#include <Language/Types.h>

static Type_Integer type_u8_v = {
    .type_id = TYPE_INTEGER,
    .size = 1,
    .is_signed = 0,
};

static Type_Integer type_u16_v = {
    .type_id = TYPE_INTEGER,
    .size = 2,
    .is_signed = 0,
};

static Type_Integer type_u32_v = {
    .type_id = TYPE_INTEGER,
    .size = 4,
    .is_signed = 0,
};

static Type_Integer type_u64_v = {
    .type_id = TYPE_INTEGER,
    .size = 8,
    .is_signed = 0,
};

static Type_Integer type_s8_v = {
    .type_id = TYPE_INTEGER,
    .size = 1,
    .is_signed = 1,
};

static Type_Integer type_s16_v = {
    .type_id = TYPE_INTEGER,
    .size = 2,
    .is_signed = 1,
};

static Type_Integer type_s32_v = {
    .type_id = TYPE_INTEGER,
    .size = 4,
    .is_signed = 1,
};

static Type_Integer type_s64_v = {
    .type_id = TYPE_INTEGER,
    .size = 8,
    .is_signed = 1,
};

static Type_Float type_fp32_v = {
    .type_id = TYPE_FLOAT,
    .size = 4,
};

static Type_Float type_fp64_v = {
    .type_id = TYPE_FLOAT,
    .size = 8,
};

Type* type_u8 = (Type*)(&type_u8_v);
Type* type_u16 = (Type*)(&type_u16_v);
Type* type_u32 = (Type*)(&type_u32_v);
Type* type_u64 = (Type*)(&type_u64_v);
Type* type_s8 = (Type*)(&type_s8_v);
Type* type_s16 = (Type*)(&type_s16_v);
Type* type_s32 = (Type*)(&type_s32_v);
Type* type_s64 = (Type*)(&type_s64_v);
Type* type_fp32 = (Type*)(&type_fp32_v);
Type* type_fp64 = (Type*)(&type_fp64_v);

Type_Array* Type_Array_create()
{
    Type_Array* self = (Type_Array*)malloc(sizeof(Type_Array));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_ARRAY;
    self->value = NULL;
    self->size = 0;
    return self;
}

Type_Pointer* Type_Pointer_create()
{
    Type_Pointer* self = (Type_Pointer*)malloc(sizeof(Type_Pointer));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_POINTER;
    self->value = NULL;
    return self;
}

Type_Tuple* Type_Tuple_create()
{
    Type_Tuple* self = (Type_Tuple*)malloc(sizeof(Type_Tuple));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_TUPLE;
    self->fields = Stack_create(sizeof(Type*));
    self->offsets = Stack_create(sizeof(size_t));
    self->alignment = 0;
    self->size = 0;
    return self;
}

size_t Type_Tuple_push_field(Type_Tuple* self, Type* type)
{
    size_t alignment = Type_alignment(type);
    size_t size = Type_size(type);

    if (alignment > self->alignment)
    {
        self->alignment = alignment;
    }

    size_t mask = self->alignment - 1;
    size_t offset = (self->size + mask) & ~mask;

    Stack_push(&self->fields, &type);
    Stack_push(&self->offsets, &offset);
    self->size = offset + size;

    return self->fields.size - 1;
}

Type_Struct* Type_Struct_create()
{
    Type_Struct* self = (Type_Struct*)malloc(sizeof(Type_Struct));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_STRUCT;
    self->ids = Map_create(sizeof(size_t));
    self->ordered = Stack_create(sizeof(String));
    self->tuple = Type_Tuple_create();
    return self;
}

Boolean Type_Struct_field_exists(Type_Struct* self, String id)
{
    return Map_get(&self->ids, id) ? True : False;
}

size_t Type_Struct_push_field(Type_Struct* self, String id, Type* type)
{
    size_t index = Type_Tuple_push_field(self->tuple, type);
    Map_set(&self->ids, id, &index);
    Stack_push(&self->ordered, &id);
    return index;
}

Type_Function* Type_Function_create()
{
    Type_Function* self = (Type_Function*)malloc(sizeof(Type_Function));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_FUNCTION;
    self->inputs = Stack_create(sizeof(Type*));
    self->output = NULL;
    return self;
}

void Type_Array_destroy(Type_Array* self)
{
    Type_destroy(self->value);
    free(self);
}

void Type_Pointer_destroy(Type_Pointer* self)
{
    Type_destroy(self->value);
    free(self);
}

void Type_Tuple_destroy(Type_Tuple* self)
{
    for (size_t i = 0; i < self->fields.size; i++)
    {
        Type_destroy(*(Type**)Stack_get(&self->fields, i));
    }

    Stack_destroy(&self->fields);
    Stack_destroy(&self->offsets);
    free(self);
}

void Type_Struct_destroy(Type_Struct* self)
{
    Map_destroy(&self->ids);
    Stack_destroy(&self->ordered);
    Type_destroy((Type*)(self->tuple));
    free(self);
}

void Type_Function_destroy(Type_Function* self)
{
    for (size_t i = 0; i < self->inputs.size; i++)
    {
        Type_destroy(*(Type**)Stack_get(&self->inputs, i));
    }

    Type_destroy(self->output);
    free(self);
}

void Type_destroy(Type* type)
{
    if (!type)
    {
        return;
    }

    switch (type->type_id)
    {
        case TYPE_ARRAY:
        {
            Type_Array_destroy((Type_Array*)type);
            break;
        }
        case TYPE_POINTER:
        {
            Type_Pointer_destroy((Type_Pointer*)type);
            break;
        }
        case TYPE_TUPLE:
        {
            Type_Tuple_destroy((Type_Tuple*)type);
            break;
        }
        case TYPE_STRUCT:
        {
            Type_Struct_destroy((Type_Struct*)type);
            break;
        }
        case TYPE_FUNCTION:
        {
            Type_Function_destroy((Type_Function*)type);
            break;
        }
    }
}

Type* Type_copy(Type* type)
{
    switch (type->type_id)
    {
        case TYPE_INTEGER:
        case TYPE_FLOAT:
        {
            return type;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)(type);
            Type_Array* copy = Type_Array_create();
            copy->size = casted->size;
            copy->value = Type_copy(casted->value);
            return (Type*)(copy);
        }
        case TYPE_POINTER:
        {
            Type_Pointer* casted = (Type_Pointer*)(type);
            Type_Pointer* copy = Type_Pointer_create();
            copy->value = Type_copy(casted->value);
            return (Type*)(copy);
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)(type);
            Type_Tuple* copy = Type_Tuple_create();

            for (size_t i = 0; i < casted->fields.size; i++)
            {
                Type_Tuple_push_field(
                    copy, Type_copy(*(Type**)(Stack_get(&casted->fields, i))));
            }

            return (Type*)copy;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;
            Type_Struct* copy = Type_Struct_create();

            for (size_t i = 0; i < casted->ordered.size; i++)
            {
                Type_Struct_push_field(
                    copy, *(String*)(Stack_get(&casted->ordered, i)),
                    Type_copy(*(Type**)(Stack_get(&casted->tuple->fields, i))));
            }

            return (Type*)copy;
        }
        case TYPE_FUNCTION:
        {
            Type_Function* casted = (Type_Function*)type;
            Type_Function* copy = Type_Function_create();

            for (size_t i = 1; i < casted->inputs.size; i++)
            {
                Type* field = Type_copy(*(Type**)Stack_get(&casted->inputs, i));
                Stack_push(&copy->inputs, &field);
            }

            copy->output = Type_copy(casted->output);
            return (Type*)copy;
        }
    }

    assert(False && "Type must be one of Type_ID enum values.");
    return NULL;
}

size_t Type_alignment(Type* type)
{
    switch (type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer* casted = (Type_Integer*)type;
            return casted->size;
        }
        case TYPE_FLOAT:
        {
            Type_Float* casted = (Type_Float*)type;
            return casted->size;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;
            return Type_alignment(casted->value);
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;
            return casted->alignment;
        }
    }

    return 8;
}

size_t Type_size(Type* type)
{
    switch (type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer* casted = (Type_Integer*)type;
            return casted->size;
        }
        case TYPE_FLOAT:
        {
            Type_Float* casted = (Type_Float*)type;
            return casted->size;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;
            return casted->size * Type_size(casted->value);
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;
            size_t size = casted->size;
            size_t mask = casted->alignment - 1;
            return (size + mask) & ~mask;
        }
    }

    return 8;
}

Boolean Type_equal(Type* a, Type* b)
{
    switch (a->type_id)
    {
        case TYPE_INTEGER:
        {
            if (b->type_id != TYPE_INTEGER)
            {
                return False;
            }

            Type_Integer* ac = (Type_Integer*)a;
            Type_Integer* bc = (Type_Integer*)b;
            return ac->size == bc->size && ac->is_signed == bc->is_signed;
        }
        case TYPE_FLOAT:
        {
            if (b->type_id != TYPE_FLOAT)
            {
                return False;
            }

            Type_Float* ac = (Type_Float*)a;
            Type_Float* bc = (Type_Float*)b;
            return ac->size == bc->size;
        }
        case TYPE_ARRAY:
        {
            if (b->type_id != TYPE_ARRAY)
            {
                return False;
            }

            Type_Array* ac = (Type_Array*)a;
            Type_Array* bc = (Type_Array*)b;
            return ac->size == bc->size && Type_equal(ac->value, bc->value);
        }
        case TYPE_POINTER:
        {
            if (b->type_id != TYPE_POINTER)
            {
                return False;
            }

            Type_Pointer* ac = (Type_Pointer*)a;
            Type_Pointer* bc = (Type_Pointer*)b;
            return Type_equal(ac->value, bc->value);
        }
        case TYPE_TUPLE:
        {
            if (b->type_id != TYPE_TUPLE)
            {
                return False;
            }

            Type_Tuple* ac = (Type_Tuple*)a;
            Type_Tuple* bc = (Type_Tuple*)b;

            if (ac->fields.size != bc->fields.size)
            {
                return False;
            }

            for (size_t i = 0; i < ac->fields.size; i++)
            {
                if (!Type_equal(*(Type**)Stack_get(&ac->fields, i),
                                *(Type**)Stack_get(&bc->fields, i)))
                {
                    return False;
                }
            }

            return True;
        }
        case TYPE_STRUCT:
        {
            if (b->type_id != TYPE_STRUCT)
            {
                return False;
            }

            Type_Struct* ac = (Type_Struct*)a;
            Type_Struct* bc = (Type_Struct*)b;

            if (ac->ordered.size != bc->ordered.size)
            {
                return False;
            }

            for (size_t i = 0; i < ac->ordered.size; i++)
            {
                String id_a = *(String*)(Stack_get(&ac->ordered, i));
                String id_b = *(String*)(Stack_get(&bc->ordered, i));

                if (!String_equal(id_a, id_b))
                {
                    return False;
                }
            }

            return Type_equal((Type*)ac->tuple, (Type*)bc->tuple);
        }
        case TYPE_FUNCTION:
        {
            if (b->type_id != TYPE_FUNCTION)
            {
                return False;
            }

            Type_Function* ac = (Type_Function*)a;
            Type_Function* bc = (Type_Function*)b;

            for (size_t i = 0; i < ac->inputs.size; i++)
            {
                if (!Type_equal(*(Type**)Stack_get(&ac->inputs, i),
                                *(Type**)Stack_get(&bc->inputs, i)))
                {
                    return False;
                }
            }

            return Type_equal(ac->output, bc->output);
        }
        default:
        {
            return False;
        }
    }
}

void Type_print(Type* type)
{
    if (!type)
    {
        return;
    }

    switch (type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer* casted = (Type_Integer*)type;

            switch (casted->size)
            {
                case 1:
                {
                    Print("%s", casted->is_signed ? "s8" : "u8");
                    break;
                }
                case 2:
                {
                    Print("%s", casted->is_signed ? "s16" : "u16");
                    break;
                }
                case 4:
                {
                    Print("%s", casted->is_signed ? "s32" : "u32");
                    break;
                }
                case 8:
                {
                    Print("%s", casted->is_signed ? "s64" : "u64");
                    break;
                }
            }

            break;
        }
        case TYPE_FLOAT:
        {
            Type_Float* casted = (Type_Float*)type;
            Print("%s", casted->size == 4 ? "fp32" : "fp64");
            break;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;
            Print("[%llu]", casted->size);
            Type_print(casted->value);
            break;
        }
        case TYPE_POINTER:
        {
            Type_Pointer* casted = (Type_Pointer*)type;
            Print("*");
            Type_print(casted->value);
            break;
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;

            Print("<");

            Type_print(*(Type**)Stack_get(&casted->fields, 0));

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                Print(", ");
                Type_print(*(Type**)Stack_get(&casted->fields, i));
            }

            Print(">");

            break;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;

            Print("{");

            Print("%s : ",
                  String_begin(*(String*)(Stack_get(&casted->ordered, 0))));
            Type_print(*(Type**)Stack_get(&casted->tuple->fields, 0));

            for (size_t i = 1; i < casted->ordered.size; i++)
            {
                Print(", ");
                Print("%s : ",
                      String_begin(*(String*)(Stack_get(&casted->ordered, i))));
                Type_print(*(Type**)Stack_get(&casted->tuple->fields, i));
            }

            Print("}");

            break;
        }
        case TYPE_FUNCTION:
        {
            Type_Function* casted = (Type_Function*)type;

            Print("(");

            if (casted->inputs.size)
            {
                Type_print(*(Type**)Stack_get(&casted->inputs, 0));

                for (size_t i = 1; i < casted->inputs.size; i++)
                {
                    Print(", ");
                    Type_print(*(Type**)Stack_get(&casted->inputs, i));
                }
            }

            Print(" -> ");

            Type_print(casted->output);

            Print(")");

            break;
        }
    }
}
