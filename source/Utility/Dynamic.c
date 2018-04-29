#include <Utility/Dynamic.h>
#include <Windows.h>

typedef void Library;

Library* Library_load(String path)
{
    return LoadLibrary(String_begin(path));
}

Library* Library_get_address(Library* library, String path)
{
    return GetProcAddress(library, String_begin(path));
}

void Library_destroy(Library* library)
{
    FreeLibrary(library);
}
