#include <Support/Dynamic.h>
#include <Windows.h>

Library* Library_load(String path)
{
    return LoadLibraryA(String_begin(path));
}

Library* Library_get_address(Library* library, String path)
{
    return GetProcAddress(library, String_begin(path));
}

void Library_destroy(Library* library)
{
    FreeLibrary(library);
}
