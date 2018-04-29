#include <Interpreter/Interpreter.h>
#include <Utility/Dynamic.h>

int main(int argc, char** argv)
{
    String_init();
    Lexer_init();

    // Interpreter_loop();

    String path = String_new("C:/Windows/System32/msvcrt.dll", 31);
    String name = String_new("printf", 7);

    Library* lib = Library_load(path);

    if (!lib)
    {
        Error("Library %s not found\n", String_begin(path));
        exit(1);
    }

    Library* ptr = Library_get_address(lib, name);

    if (!ptr)
    {
        Error("Function %s not found\n", String_begin(name));
        exit(1);
    }

    int (*ptr_printf)(const char*, ...) = ptr;

    ptr_printf("Teste\n");

    Lexer_free();
    String_free();
    return 0;
}
