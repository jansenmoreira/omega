#ifndef PANIC_H
#define PANIC_H

#include <Utility/Console.h>
#include <Utility/Standard.h>

typedef enum Error_Code
{
    Memory_Error = 1000,
    Input_Output_Error
} Error_Code;

void Panic(Error_Code code);

#endif
