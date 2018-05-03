#ifndef STANDARD_H
#define STANDARD_H

#include <assert.h>
#include <inttypes.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef float FP32;
typedef double FP64;

typedef U8* PTR;

typedef enum Boolean
{
    False,
    True
} Boolean;

#endif
