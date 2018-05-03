#include <Utility/Console.h>

#include <Windows.h>

enum
{
    BUFFER_SIZE = 100
};

static char* Read_Fallback()
{
    if (feof(stdin))
        return NULL;

    char* line = NULL;
    size_t line_size = 1;

    for (Boolean step = True; step;)
    {
        char buffer[BUFFER_SIZE + 1];

        char* ptr = fgets(buffer, BUFFER_SIZE, stdin);

        if (!ptr && (ferror(stdin) || feof(stdin) && line_size == 1))
        {
            free(line);
            return NULL;
        };

        size_t size = strlen(buffer);

        line = realloc(line, line_size - 1 + size + 1);
        memcpy(line + line_size - 1, buffer, size + 1);

        line_size += size;

        step = line[line_size - 2] != '\n' && !feof(stdin);
    }

    return line;
}

char* Read()
{
    switch (GetFileType(GetStdHandle(STD_INPUT_HANDLE)))
    {
        case FILE_TYPE_CHAR:
            break;
        case FILE_TYPE_DISK:
        case FILE_TYPE_PIPE:
            return Read_Fallback();
        default:
            return NULL;
    }

    char* line = NULL;
    size_t line_size = 1;

    for (Boolean step = True; step;)
    {
        int wide_size;

        wchar_t wide_buffer[BUFFER_SIZE];

        if (!ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), wide_buffer,
                          BUFFER_SIZE, &wide_size, NULL))
        {
            free(line);
            return NULL;
        }

        int multibyte_size = WideCharToMultiByte(CP_UTF8, 0, wide_buffer,
                                                 wide_size, NULL, 0, 0, 0);

        if (!multibyte_size)
        {
            free(line);
            return NULL;
        }

        char* multibyte_buffer =
            (char*)malloc(sizeof(char) * (multibyte_size + 1));

        if (!WideCharToMultiByte(CP_UTF8, 0, wide_buffer, wide_size,
                                 multibyte_buffer, multibyte_size, 0, 0))
        {
            free(multibyte_buffer);
            free(line);
            return NULL;
        }

        multibyte_buffer[multibyte_size] = 0;

        line = realloc(line, line_size + multibyte_size);
        memcpy(line + line_size - 1, multibyte_buffer, multibyte_size + 1);
        free(multibyte_buffer);

        line_size += multibyte_size;

        step = line[line_size - 2] != '\n';
    }

    return line;
}

static Boolean Write_Fallback(const char* format, va_list args)
{
    return vprintf(format, args) >= 0;
}

static Boolean Write(HANDLE handle, const char* format, va_list args)
{
    switch (GetFileType(handle))
    {
        case FILE_TYPE_CHAR:
            break;
        case FILE_TYPE_DISK:
        case FILE_TYPE_PIPE:
            return Write_Fallback(format, args);
        default:
            return False;
    }

    int multibyte_size = vsnprintf(NULL, 0, format, args);

    if (multibyte_size <= 0)
    {
        return !multibyte_size;
    }

    char* multibyte_buffer = (char*)malloc(sizeof(char) * (multibyte_size + 1));
    vsnprintf(multibyte_buffer, multibyte_size + 1, format, args);

    int wide_size = MultiByteToWideChar(CP_UTF8, 0, multibyte_buffer,
                                        multibyte_size, NULL, 0);

    if (!wide_size)
    {
        free(multibyte_buffer);

        return False;
    }

    wchar_t* wide_buffer = (wchar_t*)malloc(sizeof(wchar_t) * wide_size);

    if (!MultiByteToWideChar(CP_UTF8, 0, multibyte_buffer, multibyte_size,
                             wide_buffer, wide_size))
    {
        free(wide_buffer);
        free(multibyte_buffer);

        return False;
    }

    DWORD written;

    if (!WriteConsoleW(handle, wide_buffer, wide_size, &written, 0) ||
        written != wide_size)
    {
        free(wide_buffer);
        free(multibyte_buffer);

        return False;
    }

    free(wide_buffer);
    free(multibyte_buffer);

    return True;
}

Boolean Error(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    Boolean result = Write(GetStdHandle(STD_ERROR_HANDLE), format, args);

    va_end(args);

    return result;
}

Boolean Print(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    Boolean result = Write(GetStdHandle(STD_OUTPUT_HANDLE), format, args);

    va_end(args);

    return result;
}
