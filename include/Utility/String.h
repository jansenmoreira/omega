#ifndef STRING_H
#define STRING_H

#include <Utility/Standard.h>

typedef struct String
{
    size_t index;
    size_t size;
    U64 hash;
} String;

const char* String_begin(String a);

const char* String_end(String a);

Boolean String_equal(String a, String b);

Boolean String_empty(String a);

void String_init();

void String_free();

String String_new(const char* ptr, size_t size);

String String_fmt(const char* format, ...);

#endif
