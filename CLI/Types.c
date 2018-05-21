#include <CLI/Types.h>

Type_Integer type_u8 = {
    .type_id = TYPE_INTEGER,
    .size = 1,
    .is_signed = 0,
};

Type_Integer type_u16 = {
    .type_id = TYPE_INTEGER,
    .size = 2,
    .is_signed = 0,
};

Type_Integer type_u32 = {
    .type_id = TYPE_INTEGER,
    .size = 4,
    .is_signed = 0,
};

Type_Integer type_u64 = {
    .type_id = TYPE_INTEGER,
    .size = 8,
    .is_signed = 0,
};

Type_Integer type_s8 = {
    .type_id = TYPE_INTEGER,
    .size = 1,
    .is_signed = 1,
};

Type_Integer type_s16 = {
    .type_id = TYPE_INTEGER,
    .size = 2,
    .is_signed = 1,
};

Type_Integer type_s32 = {
    .type_id = TYPE_INTEGER,
    .size = 4,
    .is_signed = 1,
};

Type_Integer type_s64 = {
    .type_id = TYPE_INTEGER,
    .size = 8,
    .is_signed = 1,
};

Type_Float type_fp32 = {
    .type_id = TYPE_FLOAT,
    .size = 4,
};

Type_Float type_fp64 = {
    .type_id = TYPE_FLOAT,
    .size = 8,
};

Type_Type type_type = {
    .type_id = TYPE_TYPE,
};

Type_Type* Type_Type_create(Type* type)
{
    return &type_type;
}

Type_Integer* Type_Integer_create(size_t size, Boolean is_signed)
{
    switch (size)
    {
        case 1:
            return (is_signed) ? &type_s8 : &type_u8;
        case 2:
            return (is_signed) ? &type_s16 : &type_u16;
        case 4:
            return (is_signed) ? &type_s32 : &type_u32;
        case 8:
            return (is_signed) ? &type_s64 : &type_u64;
    }
}

Type_Float* Type_Float_create(size_t size)
{
    return (size == 4) ? &type_fp32 : &type_fp64;
}

Type_Array* Type_Array_create(Type* type, size_t size)
{
    Type_Array* self = (Type_Array*)malloc(sizeof(Type_Array));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_ARRAY;
    self->type = type;
    self->size = size;
    return self;
}

Type_Pointer* Type_Pointer_create(Type* type)
{
    Type_Pointer* self = (Type_Pointer*)malloc(sizeof(Type_Pointer));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_POINTER;
    self->type = type;
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
    return self;
}

Type_Struct* Type_Struct_create(String id)
{
    Type_Struct* self = (Type_Struct*)malloc(sizeof(Type_Struct));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_STRUCT;
    self->id = id;
    self->field_types = Stack_create(sizeof(Type*));
    self->field_ids = Stack_create(sizeof(String));
    return self;
}

Type_Function* Type_Function_create()
{
    Type_Function* self = (Type_Function*)malloc(sizeof(Type_Function));

    if (!self)
    {
        Panic(Memory_Error);
    }

    self->type_id = TYPE_FUNCTION;
    self->params = Stack_create(sizeof(Type*));
    return self;
}

void Type_Array_destroy(Type_Array* self)
{
    Type_destroy(self->type);
    free(self);
}

void Type_Pointer_destroy(Type_Pointer* self)
{
    Type_destroy(self->type);
    free(self);
}

void Type_Tuple_destroy(Type_Tuple* self)
{
    for (size_t i = 0; i < self->fields.size; i++)
    {
        Type_destroy(*(Type**)Stack_get(&self->fields, i));
    }

    Stack_destroy(&self->fields);
    free(self);
}

void Type_Struct_destroy(Type_Struct* self)
{
    for (size_t i = 0; i < self->field_types.size; i++)
    {
        Type_destroy(*(Type**)Stack_get(&self->field_types, i));
    }

    Stack_destroy(&self->field_types);
    Stack_destroy(&self->field_ids);
    free(self);
}

void Type_Function_destroy(Type_Function* self)
{
    for (size_t i = 0; i < self->params.size; i++)
    {
        Type_destroy(*(Type**)Stack_get(&self->params, i));
    }

    Stack_destroy(&self->params);
    free(self);
}

void Type_destroy(Type* type)
{
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

Type* Type_Copy(Type* type)
{
    switch (type->type_id)
    {
        case TYPE_TYPE:
        case TYPE_INTEGER:
        case TYPE_FLOAT:
        {
            return type;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;
            return (Type*)Type_Array_create(Type_Copy(casted->type),
                                            casted->size);
        }
        case TYPE_POINTER:
        {
            Type_Pointer* casted = (Type_Pointer*)type;
            return (Type*)Type_Pointer_create(Type_Copy(casted->type));
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;
            Type_Tuple* copy = Type_Tuple_create();

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                Type* field = Type_Copy(*(Type**)Stack_get(&casted->fields, i));
                Stack_push(&copy->fields, &field);
            }

            return (Type*)copy;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;
            Type_Struct* copy = Type_Struct_create(casted->id);

            for (size_t i = 1; i < casted->field_types.size; i++)
            {
                Type* field_type =
                    Type_Copy(*(Type**)Stack_get(&casted->field_types, i));

                String field_name = *(String*)Stack_get(&casted->field_ids, i);

                Stack_push(&copy->field_types, &field_type);
                Stack_push(&copy->field_ids, &field_name);
            }

            return (Type*)copy;
        }
        case TYPE_FUNCTION:
        {
            Type_Function* casted = (Type_Function*)type;
            Type_Function* copy = Type_Function_create();
            copy->return_type = Type_Copy(casted->return_type);

            for (size_t i = 1; i < casted->params.size; i++)
            {
                Type* field = Type_Copy(*(Type**)Stack_get(&casted->params, i));
                Stack_push(&copy->params, &field);
            }

            return (Type*)copy;
        }
    }

    assert(False && "Type must be one of Type_ID enum values.");
    return NULL;
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
        case TYPE_TYPE:
        {
            return 8;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;
            return casted->size * Type_size(casted->type);
        }
        case TYPE_POINTER:
        {
            return 8;
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;

            size_t size = 0;

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                size += Type_size(*(Type**)Stack_get(&casted->fields, i));
            }

            return size;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;

            size_t size = 0;

            for (size_t i = 1; i < casted->field_types.size; i++)
            {
                size += Type_size(*(Type**)Stack_get(&casted->field_types, i));
            }

            return size;
        }
        case TYPE_FUNCTION:
        {
            return 8;
        }
        default:
        {
            return False;
        }
    }
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
        case TYPE_TYPE:
        {
            return b->type_id == TYPE_TYPE;
        }
        case TYPE_ARRAY:
        {
            if (b->type_id != TYPE_ARRAY)
            {
                return False;
            }

            Type_Array* ac = (Type_Array*)a;
            Type_Array* bc = (Type_Array*)b;
            return ac->size == bc->size && Type_equal(ac->type, bc->type);
        }
        case TYPE_POINTER:
        {
            if (b->type_id != TYPE_POINTER)
            {
                return False;
            }

            Type_Pointer* ac = (Type_Pointer*)a;
            Type_Pointer* bc = (Type_Pointer*)b;
            return Type_equal(ac->type, bc->type);
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

            if (ac->field_types.size != bc->field_types.size)
            {
                return False;
            }

            for (size_t i = 0; i < ac->field_types.size; i++)
            {
                if (!Type_equal(*(Type**)Stack_get(&ac->field_types, i),
                                *(Type**)Stack_get(&bc->field_types, i)) ||
                    !String_equal(*(String*)Stack_get(&ac->field_ids, i),
                                  *(String*)Stack_get(&bc->field_ids, i)))
                {
                    return False;
                }
            }

            return True;
        }
        case TYPE_FUNCTION:
        {
            if (b->type_id != TYPE_FUNCTION)
            {
                return False;
            }

            Type_Function* ac = (Type_Function*)a;
            Type_Function* bc = (Type_Function*)b;

            if (!Type_equal(ac->return_type, bc->return_type))
            {
                return False;
            }

            if (ac->params.size != bc->params.size)
            {
                return False;
            }

            for (size_t i = 0; i < ac->params.size; i++)
            {
                if (!Type_equal(*(Type**)Stack_get(&ac->params, i),
                                *(Type**)Stack_get(&bc->params, i)))
                {
                    return False;
                }
            }

            return True;
        }
        default:
        {
            return False;
        }
    }
}

void Type_print(Type* type)
{
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
        case TYPE_TYPE:
        {
            Print("Type");
            break;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;
            Print("[%llu]", casted->size);
            Type_print(casted->type);
            break;
        }
        case TYPE_POINTER:
        {
            Type_Pointer* casted = (Type_Pointer*)type;
            Print("*");
            Type_print(casted->type);
            break;
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;

            Print("[");

            Type_print(*(Type**)Stack_get(&casted->fields, 0));

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                Print(", ");
                Type_print(*(Type**)Stack_get(&casted->fields, i));
            }

            Print("]");

            break;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;

            Print("{");

            Type_print(*(Type**)Stack_get(&casted->field_types, 0));

            for (size_t i = 1; i < casted->field_types.size; i++)
            {
                Print(", ");
                Type_print(*(Type**)Stack_get(&casted->field_types, i));
            }

            Print("}");

            break;
        }
        case TYPE_FUNCTION:
        {
            Type_Function* casted = (Type_Function*)type;

            Print("(");

            Type_print(*(Type**)Stack_get(&casted->params, 0));

            for (size_t i = 1; i < casted->params.size; i++)
            {
                Print(", ");
                Type_print(*(Type**)Stack_get(&casted->params, i));
            }

            Print(" -> ");

            Type_print(casted->return_type);

            Print(")");

            break;
        }
    }
}

void Type_print_value(Type* type, void* value)
{
    switch (type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer* casted = (Type_Integer*)type;

            switch (casted->size)
            {
                case 1:
                {
                    if (!casted->is_signed)
                    {
                        Print("%" PRIu8, *((U8*)value));
                    }
                    else
                    {
                        Print("%" PRIi8, *((S8*)value));
                    }

                    break;
                }
                case 2:
                {
                    if (!casted->is_signed)
                    {
                        Print("%" PRIu16, *((U16*)value));
                    }
                    else
                    {
                        Print("%" PRIi16, *((S16*)value));
                    }

                    break;
                }
                case 4:
                {
                    if (!casted->is_signed)
                    {
                        Print("%" PRIu32, *((U32*)value));
                    }
                    else
                    {
                        Print("%" PRIi32, *((S32*)value));
                    }

                    break;
                }
                case 8:
                {
                    if (!casted->is_signed)
                    {
                        Print("%" PRIu64, *((U64*)value));
                    }
                    else
                    {
                        Print("%" PRIi64, *((S64*)value));
                    }

                    break;
                }
            }

            break;
        }
        case TYPE_FLOAT:
        {
            Type_Float* casted = (Type_Float*)type;

            switch (casted->size)
            {
                case 4:
                {
                    Print("%f", *((float*)value));
                    break;
                }
                case 8:
                {
                    Print("%lf", *((double*)value));
                    break;
                }
            }

            break;
        }
        case TYPE_TYPE:
        {
            Type* type = (Type*)value;
            Print("Type => ");
            Type_print(type);
            break;
        }
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;

            Print("[ ");

            for (size_t i = 0; i < casted->size; i++)
            {
                Type_print_value(casted->type, value);
                value = ((U8*)value) + Type_size(casted->type);
                Print(", ");
            }

            Print("]");

            break;
        }
        case TYPE_POINTER:
        case TYPE_FUNCTION:
        {
            Print("%016" PRIX64, *(U64**)(value));
            break;
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;

            Print("[ ");

            for (size_t i = 0; i < casted->fields.size; i++)
            {
                Type* field_type = *(Type**)Stack_get(&casted->fields, i);
                Type_print_value(field_type, value);
                value = ((U8*)value) + Type_size(field_type);
                Print(", ");
            }

            Print("]");
            break;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;

            Print("{ ");

            for (size_t i = 0; i < casted->field_types.size; i++)
            {
                Print("%s : ",
                      String_begin(*(String*)Stack_get(&casted->field_ids, i)));

                Type* field_type = *(Type**)Stack_get(&casted->field_types, i);

                Type_print_value(field_type, value);

                value = ((U8*)value) + Type_size(field_type);
                Print(", ");
            }

            Print("}");
            break;
        }
    }
}
