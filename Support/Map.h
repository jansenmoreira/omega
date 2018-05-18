#ifndef MAP_H
#define MAP_H

#include <Support/Panic.h>
#include <Support/String.h>

typedef struct Map
{
    String* keys;
    U8* values;
    size_t size;
    size_t capacity;
    size_t sizeof_value;
} Map;

Map Map_create(size_t value_size);

void Map_destroy(Map* self);

void* Map_set(Map* self, String key, void* value);

void* Map_get(Map* self, String key);

#endif
