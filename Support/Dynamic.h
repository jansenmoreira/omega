#ifndef DYNAMIC_H
#define DYNAMIC_h

#include <Support/Standard.h>
#include <Support/String.h>

typedef void Library;

Library* Library_load(String path);

Library* Library_get_address(Library* library, String name);

void Library_destroy(Library* library);

#endif
