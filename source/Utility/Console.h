#ifndef CONSOLE_H
#define CONSOLE_H

#include <Utility/Standard.h>

Memory(char *) Read();

Boolean Error(const char *format, ...);

Boolean Print(const char *format, ...);

#endif
