#include <Support/Panic.h>

void Panic(Error_Code code)
{
    const char *msg;

    switch (code)
    {
        case Memory_Error:
            msg = "Failed to allocate memory";
            break;
        case Input_Output_Error:
            msg = "Failed to perform an Input or Output";
            break;
		default:
			msg = "Unknown error";
			break;
    }

    Error("Fatal Error (%d): %s.\n", code, msg);
    exit(code);
}
