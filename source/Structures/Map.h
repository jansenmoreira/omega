#ifndef MAP_H
#define MAP_H

#include <Utility/Panic.h>
#include <Utility/String.h>

typedef struct Map
{
    String *keys;
    u8 *values;
    size_t size;
    size_t capacity;
    size_t sizeof_value;
} Map;

Map Map_create(size_t value_size);

void Map_destroy(Map *self);

void *Map_set(Map *self, String key, void *value);

void *Map_get(Map *self, String key);

#define MAP(T) Map
#define MAP_CREATE(T) (Map_create(sizeof(T)))
#define MAP_DESTROY(T, self) (Map_destroy(&self))
#define MAP_SET(T, self, key, value) ((T *)Map_set(&self, key, (void *)&value))
#define MAP_GET(T, self, key) ((T *)Map_get(&self, key))

#endif
