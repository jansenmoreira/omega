#include <Structures/Map.h>

Map Map_create(size_t sizeof_value)
{
    size_t initial_capacity = 4;
    Map self;
    self.sizeof_value = sizeof_value;
    self.values = (U8*)malloc(self.sizeof_value * initial_capacity);
    self.keys = (String*)malloc(sizeof(String) * initial_capacity);

    if (!self.values || !self.keys)
        Panic(Memory_Error);

    self.size = 0;
    self.capacity = initial_capacity;
    memset(self.keys, 0, sizeof(String) * initial_capacity);

    return self;
}

void Map_destroy(Map* self)
{
    free(self->values);
    free(self->keys);
    self->values = NULL;
    self->keys = NULL;
}

void* Map_get(Map* self, String key)
{
    size_t k = key.hash % self->capacity;
    size_t j = 1;

    for (;;)
    {
        if (String_empty(self->keys[k]))
        {
            return NULL;
        }
        if (String_equal(self->keys[k], key))
        {
            return self->values + self->sizeof_value * k;
        }

        k = (k + j) % self->capacity;
        j += 1;
    }
}

void* Map_set(Map* self, String key, void* value)
{
    if (self->size >= (self->capacity * 3) >> 2)
    {
        String* tmp_keys = NULL;
        U8* tmp_values = NULL;
        size_t tmp_capacity = self->capacity << 1;

        tmp_keys = (String*)malloc(sizeof(String) * tmp_capacity);
        tmp_values = (U8*)malloc(self->sizeof_value * tmp_capacity);

        if (!tmp_keys || !tmp_values)
            Panic(Memory_Error);

        memset(tmp_keys, 0, sizeof(String) * tmp_capacity);

        for (size_t i = 0; i < self->capacity; i++)
        {
            if (String_empty(self->keys[i]))
            {
                continue;
            }

            size_t k = self->keys[i].hash % tmp_capacity;
            size_t j = 1;

            for (;;)
            {
                if (String_empty(tmp_keys[k]))
                {
                    tmp_keys[k] = self->keys[i];
                    memcpy(tmp_values + self->sizeof_value * k,
                           self->values + self->sizeof_value * i,
                           self->sizeof_value);
                    break;
                }

                k = (k + j) % tmp_capacity;
                j += 1;
            }
        }

        free(self->keys);
        free(self->values);
        self->keys = tmp_keys;
        self->values = tmp_values;
        self->capacity = tmp_capacity;
    }

    size_t k = key.hash % self->capacity;
    size_t j = 1;

    for (;;)
    {
        if (String_empty(self->keys[k]))
        {
            self->keys[k] = key;
            memcpy(self->values + self->sizeof_value * k, value,
                   self->sizeof_value);
            self->size += 1;
            return &self->values[k];
        }
        else if (String_equal(self->keys[k], key))
        {
            memcpy(self->values + self->sizeof_value * k, value,
                   self->sizeof_value);
            return &self->values[k];
        }

        k = (k + j) % self->capacity;
        j += 1;
    }
}
