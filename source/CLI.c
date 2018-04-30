#include <Interpreter/Interpreter.h>
#include <Utility/Dynamic.h>

int main(int argc, char** argv)
{
    String_init();
    Lexer_init();

    Interpreter_loop();

    Lexer_free();
    String_free();
    return 0;
}
