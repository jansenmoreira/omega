#ifndef FILE_H
#define FILE_H

#include <Structures/Stack.h>
#include <Utility/String.h>

int File_utf_width;

typedef struct File
{
    char *text;
    STACK(u64) lines;
    String path;
} File;

typedef struct Position
{
    u64 line;
    u64 column;
} Position;

Boolean File_open(File *self, String path);

void File_destroy(File *self);

Position File_position(File *self, u64 at, int is_begin);

#endif
