#include <Language/Interpreter.h>
#include <Language/Parser.h>
#include <Machine/Execution.h>
#include <Support/Performance.h>
#include <Support/String.h>

int main(int argc, char** argv)
{
    String_init();
    File_init();
    Lexer_init();

    AST* ast = Parse(String_new("samples/demo.omega", 18));

    Print("\n\n--------------------------------------------------\n\n");
    AST_print((AST*)(ast));
    Print("\n\n--------------------------------------------------\n\n");

    Interpreter interpreter = Interpreter_create();
    Interpreter_run(&interpreter, ast);

    AST_destroy((AST*)(ast));

    Lexer_exit();
    File_exit();
    String_exit();

    return 0;
}
