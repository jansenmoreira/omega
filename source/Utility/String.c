#include <Utility/String.h>

#define ROTL64(x, y) (x << y) | (x >> (64 - y))

typedef struct String_Manager
{
    char* buffer;
    size_t buffer_size;
    size_t buffer_capacity;

    String* set;
    size_t set_size;
    size_t set_capacity;

    U32 seed;
} String_Manager;

static String_Manager manager;

static U64 FMIX64(const U64 x)
{
    U64 k = x;
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;
    return k;
}

static void hash(const void* key, const U64 length, const U32 seed, U64* high,
                 U64* low)
{
    const U8* data = (const U8*)key;
    const U64 blocksCount = length / 16;

    U64 h1 = seed;
    U64 h2 = seed;

    U64 k1;
    U64 k2;

    const U64 c1 = 0x87c37b91114253d5ULL;
    const U64 c2 = 0x4cf5ad432745937fULL;

    const U64* blocks = (const U64*)data;

    for (U64 i = 0; i < blocksCount; i += 2)
    {
        k1 = blocks[i];
        k2 = blocks[i + 1];

        k1 *= c1;
        k1 = ROTL64(k1, 31);
        k1 *= c2;
        h1 ^= k1;
        h1 = ROTL64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = ROTL64(k2, 33);
        k2 *= c1;
        h2 ^= k2;
        h2 = ROTL64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    const U8* tail = (const U8*)(data + blocksCount * 16);

    k1 = 0;
    k2 = 0;

    switch (length & 15)
    {
        case 15:
            k2 ^= ((U64)tail[14]) << 48;
        case 14:
            k2 ^= ((U64)tail[13]) << 40;
        case 13:
            k2 ^= ((U64)tail[12]) << 32;
        case 12:
            k2 ^= ((U64)tail[11]) << 24;
        case 11:
            k2 ^= ((U64)tail[10]) << 16;
        case 10:
            k2 ^= ((U64)tail[9]) << 8;
        case 9:
            k2 ^= ((U64)tail[8]) << 0;
            k2 *= c2;
            k2 = ROTL64(k2, 33);
            k2 *= c1;
            h2 ^= k2;
        case 8:
            k1 ^= ((U64)tail[7]) << 56;
        case 7:
            k1 ^= ((U64)tail[6]) << 48;
        case 6:
            k1 ^= ((U64)tail[5]) << 40;
        case 5:
            k1 ^= ((U64)tail[4]) << 32;
        case 4:
            k1 ^= ((U64)tail[3]) << 24;
        case 3:
            k1 ^= ((U64)tail[2]) << 16;
        case 2:
            k1 ^= ((U64)tail[1]) << 8;
        case 1:
            k1 ^= ((U64)tail[0]) << 0;
            k1 *= c1;
            k1 = ROTL64(k1, 31);
            k1 *= c2;
            h1 ^= k1;
    }

    h1 ^= length;
    h2 ^= length;
    h1 += h2;
    h2 += h1;
    h1 = FMIX64(h1);
    h2 = FMIX64(h2);
    h1 += h2;
    h2 += h1;

    *high = h2;
    *low = h1;
}

const char* String_begin(String a)
{
    return manager.buffer + a.index;
}

const char* String_end(String a)
{
    return manager.buffer + a.index + a.size;
}

Boolean String_empty(String a)
{
    return !a.index;
}

Boolean String_equal(String a, String b)
{
    return a.index == b.index;
}

void String_init()
{
    manager.buffer_size = 1;
    manager.buffer_capacity = 32;
    manager.set_size = 0;
    manager.set_capacity = 4;
    manager.seed = (U32)time(NULL);

    manager.buffer = (char*)malloc(sizeof(char) * manager.buffer_capacity);
    manager.set = (String*)malloc(sizeof(String) * manager.set_capacity);

    if (!manager.buffer || !manager.set)
    {
        fputs("Fatal Error (1000): Failed to allocate memory.", stderr);
        exit(1000);
    }

    memset(manager.set, 0, sizeof(String) * manager.set_capacity);
    manager.buffer[0] = 0;
}

void String_free()
{
    free(manager.buffer);
    free(manager.set);
    manager.buffer = NULL;
    manager.set = NULL;
}

String String_new(const char* ptr, size_t size)
{
    if (!size)
    {
        String string;
        string.index = 0;
        string.size = size;
        string.hash = 0;

        return string;
    }

    if (manager.set_size > (manager.set_capacity >> 2) * 3)
    {
        String* tmp = NULL;
        size_t tmp_capacity = manager.set_capacity << 1;

        tmp = (String*)malloc(sizeof(String) * tmp_capacity);

        if (!tmp)
        {
            fputs("Fatal Error (1000): Failed to allocate memory.", stderr);
            exit(1000);
        }

        memset(tmp, 0, sizeof(String) * tmp_capacity);

        for (size_t i = 0; i < manager.set_capacity; i++)
        {
            if (!manager.set[i].index)
            {
                continue;
            }

            size_t k = manager.set[i].hash % tmp_capacity;
            size_t j = 1;

            for (;;)
            {
                if (!tmp[k].index || String_equal(tmp[k], manager.set[i]))
                {
                    tmp[k] = manager.set[i];
                    break;
                }

                k = (k + j) % tmp_capacity;
                j += 1;
            }
        }

        free(manager.set);
        manager.set = tmp;
        manager.set_capacity = tmp_capacity;
    }

    U64 hash_high, hash_low;
    hash(ptr, size, manager.seed, &hash_high, &hash_low);

    size_t k = hash_high % manager.set_capacity;
    size_t j = 1;

    for (;;)
    {
        if (!manager.set[k].index)
        {
            if (manager.buffer_size + size + 1 > manager.buffer_capacity)
            {
                char* tmp = NULL;

                size_t tmp_capacity = (manager.buffer_size + size + 1) << 1;
                tmp = (char*)malloc(sizeof(char) * tmp_capacity);

                if (!tmp)
                {
                    fputs("Fatal Error (1000): Failed to allocate memory.",
                          stderr);
                    exit(1000);
                }

                memcpy(tmp, manager.buffer, sizeof(char) * manager.buffer_size);

                free(manager.buffer);
                manager.buffer = tmp;
                manager.buffer_capacity = tmp_capacity;
            }

            memcpy(manager.buffer + manager.buffer_size, ptr,
                   sizeof(char) * size);

            size_t index = manager.buffer_size;
            manager.buffer_size += size + 1;
            manager.buffer[index + size] = 0;

            String string;
            string.index = index;
            string.size = size;
            string.hash = hash_high;

            manager.set[k] = string;
            manager.set_size += 1;

            return string;
        }
        else if (!String_empty(manager.set[k]) && size == manager.set[k].size &&
                 strncmp(String_begin(manager.set[k]), ptr, size) == 0)
        {
            return manager.set[k];
        }

        k = (k + j) % manager.set_capacity;
        j += 1;
    }
}
