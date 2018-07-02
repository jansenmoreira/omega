#ifndef CONSOLE_H
#define CONSOLE_H

#include <Support/Standard.h>

char* Read();

Boolean Error_va(const char* format, va_list args);

Boolean Error(const char* format, ...);

Boolean Print(const char* format, ...);

#endif
