#include <CLI/Interpreter.h>
#include <Machine/Execution.h>
#include <Support/Performance.h>

int main(int argc, char** argv)
{
    String_init();

    Interpreter_loop();

    String_free();

    return 0;
}
