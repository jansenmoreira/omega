#include <Interpreter/Types.h>

Type_Integer* Type_Integer_create(size_t size, int is_signed)
{
    Type_Integer* self = (Type_Integer*)malloc(sizeof(Type_Integer));

    if (!self)
        Panic(Memory_Error);

    self->type_id = TYPE_INTEGER;
    self->size = size;
    self->is_signed = is_signed;
    return self;
}

Type_Float* Type_Float_create(size_t size)
{
    Type_Float* self = (Type_Float*)malloc(sizeof(Type_Float));

    if (!self)
        Panic(Memory_Error);

    self->type_id = TYPE_FLOAT;
    self->size = size;
    return self;
}

Type_Array* Type_Array_create(Type* type, size_t size)
{
    Type_Array* self = (Type_Array*)malloc(sizeof(Type_Array));

    if (!self)
        Panic(Memory_Error);

    self->type_id = TYPE_ARRAY;
    self->type = type;
    self->size = size;
    return self;
}

Type_Pointer* Type_Pointer_create(Type* type)
{
    Type_Pointer* self = (Type_Pointer*)malloc(sizeof(Type_Pointer));

    if (!self)
        Panic(Memory_Error);

    self->type_id = TYPE_POINTER;
    self->type = type;
    return self;
}

Type_Tuple* Type_Tuple_create()
{
    Type_Tuple* self = (Type_Tuple*)malloc(sizeof(Type_Tuple));

    if (!self)
        Panic(Memory_Error);

    self->type_id = TYPE_TUPLE;
    self->fields = STACK_CREATE(Type*);
    return self;
}

Type_Struct* Type_Struct_create()
{
    Type_Struct* self = (Type_Struct*)malloc(sizeof(Type_Struct));

    if (!self)
        Panic(Memory_Error);

    self->type_id = TYPE_STRUCT;
    self->fields = STACK_CREATE(Type*);
    self->ids = STACK_CREATE(String);
    return self;
}

Type_Function* Type_Function_create()
{
    Type_Function* self = (Type_Function*)malloc(sizeof(Type_Function));

    if (!self)
        Panic(Memory_Error);

    self->type_id = TYPE_FUNCTION;
    self->params = STACK_CREATE(Type*);
    return self;
}

void Type_Integer_destroy(Type_Integer* self)
{
    free(self);
}

void Type_Float_destroy(Type_Float* self)
{
    free(self);
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
        Type_destroy(STACK_GET(Type*, self->fields, i));
    }

    STACK_DESTROY(Type*, self->fields);
    free(self);
}

void Type_Struct_destroy(Type_Struct* self)
{
    for (size_t i = 0; i < self->fields.size; i++)
    {
        Type_destroy(STACK_GET(Type*, self->fields, i));
    }

    STACK_DESTROY(Type*, self->fields);
    STACK_DESTROY(String, self->ids);
    free(self);
}

void Type_Function_destroy(Type_Function* self)
{
    for (size_t i = 0; i < self->params.size; i++)
    {
        Type_destroy(STACK_GET(Type*, self->params, i));
    }

    STACK_DESTROY(Type*, self->params);
    free(self);
}

void Type_destroy(Type* type)
{
    switch (type->type_id)
    {
        case TYPE_INTEGER:
        {
            Type_Integer_destroy((Type_Integer*)type);
            break;
        }
        case TYPE_FLOAT:
        {
            Type_Float_destroy((Type_Float*)type);
            break;
        }
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
        case TYPE_INTEGER:
        {
            Type_Integer* casted = (Type_Integer*)type;
            return (Type*)Type_Integer_create(casted->size, casted->is_signed);
        }
        case TYPE_FLOAT:
        {
            Type_Float* casted = (Type_Float*)type;
            return (Type*)Type_Float_create(casted->size);
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
                Type* field = Type_Copy(STACK_GET(Type*, casted->fields, i));
                STACK_PUSH(Type*, copy->fields, field);
            }

            return (Type*)copy;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;
            Type_Struct* copy = Type_Struct_create();

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                Type* field_type =
                    Type_Copy(STACK_GET(Type*, casted->fields, i));

                String field_name = STACK_GET(String, casted->ids, i);

                STACK_PUSH(Type*, copy->fields, field_type);
                STACK_PUSH(String, copy->ids, field_name);
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
                Type* field = Type_Copy(STACK_GET(Type*, casted->params, i));
                STACK_PUSH(Type*, copy->params, field);
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
                size += Type_size(STACK_GET(Type*, casted->fields, i));
            }

            return size;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;

            size_t size = 0;

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                size += Type_size(STACK_GET(Type*, casted->fields, i));
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
                if (!Type_equal(STACK_GET(Type*, ac->fields, i),
                                STACK_GET(Type*, bc->fields, i)))
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

            if (ac->fields.size != bc->fields.size)
            {
                return False;
            }

            for (size_t i = 0; i < ac->fields.size; i++)
            {
                if (!Type_equal(STACK_GET(Type*, ac->fields, i),
                                STACK_GET(Type*, bc->fields, i)) ||
                    !String_equal(STACK_GET(String, ac->ids, i),
                                  STACK_GET(String, bc->ids, i)))
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
                if (!Type_equal(STACK_GET(Type*, ac->params, i),
                                STACK_GET(Type*, bc->params, i)))
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

            Type_print(STACK_GET(Type*, casted->fields, 0));

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                Print(", ");
                Type_print(STACK_GET(Type*, casted->fields, i));
            }

            Print("]");

            break;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;

            Print("{");

            Type_print(STACK_GET(Type*, casted->fields, 0));

            for (size_t i = 1; i < casted->fields.size; i++)
            {
                Print(", ");
                Type_print(STACK_GET(Type*, casted->fields, i));
            }

            Print("}");

            break;
        }
        case TYPE_FUNCTION:
        {
            Type_Function* casted = (Type_Function*)type;

            Print("(");

            Type_print(STACK_GET(Type*, casted->params, 0));

            for (size_t i = 1; i < casted->params.size; i++)
            {
                Print(", ");
                Type_print(STACK_GET(Type*, casted->params, i));
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
                        Print("%" PRIu8, *((u8*)value));
                    }
                    else
                    {
                        Print("%" PRIi8, *((s8*)value));
                    }

                    break;
                }
                case 2:
                {
                    if (!casted->is_signed)
                    {
                        Print("%" PRIu16, *((u16*)value));
                    }
                    else
                    {
                        Print("%" PRIi16, *((s16*)value));
                    }

                    break;
                }
                case 4:
                {
                    if (!casted->is_signed)
                    {
                        Print("%" PRIu32, *((u32*)value));
                    }
                    else
                    {
                        Print("%" PRIi32, *((s32*)value));
                    }

                    break;
                }
                case 8:
                {
                    if (!casted->is_signed)
                    {
                        Print("%" PRIu64, *((u64*)value));
                    }
                    else
                    {
                        Print("%" PRIi64, *((s64*)value));
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
        case TYPE_ARRAY:
        {
            Type_Array* casted = (Type_Array*)type;

            Print("[ ");

            for (size_t i = 0; i < casted->size; i++)
            {
                Type_print_value(casted->type, value);
                value = ((u8*)value) + Type_size(casted->type);
                Print(", ");
            }

            Print("]");

            break;
        }
        case TYPE_POINTER:
        case TYPE_FUNCTION:
        {
            Print("%016" PRIX64, *(u64 **)(value));
            break;
        }
        case TYPE_TUPLE:
        {
            Type_Tuple* casted = (Type_Tuple*)type;

            Print("[ ");

            for (size_t i = 0; i < casted->fields.size; i++)
            {
                Type* field_type = STACK_GET(Type*, casted->fields, i);
                Type_print_value(field_type, value);
                value = ((u8*)value) + Type_size(field_type);
                Print(", ");
            }

            Print("]");
            break;
        }
        case TYPE_STRUCT:
        {
            Type_Struct* casted = (Type_Struct*)type;

            Print("{ ");

            for (size_t i = 0; i < casted->fields.size; i++)
            {
                Print("%s : ", String_begin(STACK_GET(String, casted->ids, i)));
                Type* field_type = STACK_GET(Type*, casted->fields, i);
                Type_print_value(field_type, value);
                value = ((u8*)value) + Type_size(field_type);
                Print(", ");
            }

            Print("}");
            break;
        }
    }
}
