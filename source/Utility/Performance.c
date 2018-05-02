#include <Utility/Performance.h>
#include <Windows.h>

double frequency;
static LARGE_INTEGER start;

Boolean Clock_start()
{
    LARGE_INTEGER li_frequency;

    if (!QueryPerformanceFrequency(&li_frequency))
    {
        return False;
    }

    frequency = li_frequency.QuadPart / 1000.0;

    QueryPerformanceCounter(&start);

    return True;
}

double Clock_get()
{
    LARGE_INTEGER now;

    QueryPerformanceCounter(&now);

    return (now.QuadPart - start.QuadPart) / frequency;
}
