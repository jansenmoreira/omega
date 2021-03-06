#include <Support/Console.h>
#include <Support/File.h>

static Map files;

void File_init()
{
    files = Map_create(sizeof(File*));
}

void File_exit()
{
    for (size_t i = 0; i < files.size; i++)
    {
        if (!String_empty(files.keys[i]))
        {
            File* self = ((File**)(files.values))[i];
            Stack_destroy(&self->lines);
            free(self->text);
        }
    }

    Map_destroy(&files);
}

Boolean File_exists(String path)
{
    if (String_empty(path))
    {
        return False;
    }

    File** ptr = (File**)(Map_get(&files, path));

    if (ptr && *ptr)
    {
        return True;
    }

    return False;
}

File* File_open(String path)
{
    if (String_empty(path))
    {
        return NULL;
    }

    File** ptr = (File**)(Map_get(&files, path));

    if (ptr && *ptr)
    {
        return *ptr;
    }

    File* file = (File*)malloc(sizeof(File));

    file->lines = Stack_create(sizeof(U64));

    FILE* stream;

    if (fopen_s(&stream, String_begin(path), "rb"))
    {
        return NULL;
    }

    U64 value = 0;
    Stack_push(&file->lines, &value);

    int c = fgetc(stream);
    int at = 0;

    for (; c != EOF; at++, c = fgetc(stream))
    {
        if (c == '\n')
        {
            value = at + 1;
            Stack_push(&file->lines, &value);
        }
    }

    if (ferror(stream))
    {
        Panic(Memory_Error);
    }

    file->text = (char*)malloc(sizeof(char) * (at + 1));

    if (!file->text)
        Panic(Input_Output_Error);

    file->text[at] = 0;
    file->path = path;

    if (fseek(stream, 0, SEEK_SET))
        Panic(Input_Output_Error);

    size_t read = fread(file->text, at, 1, stream);

    if (read != at && ferror(stream))
        Panic(Input_Output_Error);

    if (fclose(stream) == EOF)
        Panic(Input_Output_Error);

    Map_set(&files, path, &file);

    return file;
}

int File_utf_width = 32;

Position File_position(File* self, U64 at, int is_begin)
{
    Position position;

    size_t count = self->lines.size, step, it = 0, first = 0;

    while (count > 0)
    {
        step = count / 2;
        it = first + step;

        if (*(U64*)(Stack_get(&self->lines, it)) < at)
        {
            first = it + 1;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    position.line = first;

    if (*(U64*)(Stack_get(&self->lines, position.line)) == at)
    {
        position.line += 1;
    }

    position.column = 0;

    for (U64 i = *(U64*)(Stack_get(&self->lines, position.line - 1));
         i < at || (!is_begin && i <= at);)
    {
        switch (File_utf_width)
        {
            case 8:
            {
                position.column += 1;
                i += 1;

                break;
            }

            case 16:
            {
                int width = 0;

                switch (self->text[i] & 0xF0)
                {
                    case 0xF0:
                        width += 1;
                    case 0xE0:
                        width += 1;
                    case 0xC0:
                    case 0xD0:
                        width += 1;
                    default:
                        width += 1;
                }

                U32 code_page;

                switch (self->text[i] & 0xF0)
                {
                    case 0xC0:
                    case 0xD0:
                        code_page = ((self->text[i] & 0x1F) << 6) +
                                    (self->text[i + 1] & 0x3F);
                        break;
                    case 0xE0:
                        code_page = ((self->text[i] & 0x0F) << 12) +
                                    ((self->text[i + 1] & 0x3F) << 6) +
                                    (self->text[i + 2] & 0x3F);
                        break;
                    case 0xF0:
                        code_page = ((self->text[i] & 0x07) << 18) +
                                    ((self->text[i + 1] & 0x3F) << 12) +
                                    ((self->text[i + 2] & 0x3F) << 6) +
                                    (self->text[i + 3] & 0x3F);
                        break;
                    default:
                        code_page = self->text[0];
                }

                if (code_page >= 0x0000 && code_page <= 0xD7FF ||
                    code_page >= 0xE000 && code_page <= 0xFFFF)
                {
                    position.column += 1;
                }
                else if (code_page >= 0x10000 && code_page <= 0x10FFFF)
                {
                    position.column += 2;
                }

                i += width;

                break;
            }

            case 32:
            {
                switch (self->text[i] & 0xF0)
                {
                    case 0xF0:
                        i += 1;
                    case 0xE0:
                        i += 1;
                    case 0xC0:
                    case 0xD0:
                        i += 1;
                    default:
                        i += 1;
                }

                position.column += 1;

                break;
            }
        }
    }

    if (is_begin)
    {
        position.column += 1;
    }

    return position;
}
