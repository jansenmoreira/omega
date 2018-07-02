#include <Language/Parser.h>
#include <Machine/Execution.h>
#include <Support/Performance.h>
#include <Support/String.h>

int main(int argc, char** argv)
{
    String_init();
    Lexer_init();

    AST* ast = Parse(String_new("samples/demo.omega", 18));

    Print("\n\n--------------------------------------------------\n\n");
    AST_print((AST*)(ast));
    AST_destroy((AST*)(ast));

    Lexer_free();
    String_free();

    return 0;
}
