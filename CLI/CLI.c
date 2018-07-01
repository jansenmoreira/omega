#include <Machine/Execution.h>
#include <Parser/Parser.h>
#include <Support/Performance.h>
#include <Support/String.h>

int main(int argc, char** argv)
{
    String_init();

    Parser_loop();

    String_free();

    return 0;
}
