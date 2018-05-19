#include <CLI/Interpreter.h>
#include <Machine/Execution.h>
#include <Support/Performance.h>

void abc()
{
    printf("Teste");
}

int main(int argc, char** argv)
{
    String_init();

    Interpreter_loop();

    String_free();

    return 0;
}
