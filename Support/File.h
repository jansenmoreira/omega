#ifndef FILE_H
#define FILE_H

#include <Support/Stack.h>
#include <Support/Standard.h>
#include <Support/String.h>

int File_utf_width;

typedef struct File
{
    char* text;
    Stack lines;
    String path;
} File;

typedef struct Position
{
    U64 line;
    U64 column;
} Position;

Boolean File_open(File* self, String path);

void File_destroy(File* self);

Position File_position(File* self, U64 at, int is_begin);

#endif
