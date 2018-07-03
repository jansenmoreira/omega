#ifndef FILE_H
#define FILE_H

#include <Support/Map.h>
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

void File_init();

Boolean File_exists(String path);

File* File_open(String path);

Position File_position(File* self, U64 at, int is_begin);

void File_exit();

#endif
