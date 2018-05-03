#ifndef CONSOLE_H
#define CONSOLE_H

#include <Utility/Standard.h>

char* Read();

Boolean Error(const char* format, ...);

Boolean Print(const char* format, ...);

#endif
