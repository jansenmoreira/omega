cmake_minimum_required(VERSION 3.0)

project(omega)

include_directories(source)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NATVIS:${CMAKE_SOURCE_DIR}/omega.natvis")

find_package(PythonInterp REQUIRED)

add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/UCD.py --ucd ${CMAKE_SOURCE_DIR}/scripts/ucd.all.flat.zip --output ${PROJECT_BINARY_DIR}/source
    DEPENDS scripts/UCD.py
    OUTPUT  source/Utility/UCD.c
    COMMENT Generating Utility/UCD.c
)

add_executable(
    omega-cli

    source/CLI.c
    
    source/Interpreter/Types.c
    source/Interpreter/Types.h
    source/Interpreter/Expression.c
    source/Interpreter/Expression.h
    source/Interpreter/Lexer.c
    source/Interpreter/Lexer.h
    source/Interpreter/Machine.c
    source/Interpreter/Machine.h
    source/Interpreter/Interpreter.c
    source/Interpreter/Interpreter.h
    
    source/Machine/Execution.c
    source/Machine/Execution.h
    source/Machine/Machine.c
    source/Machine/Machine.h

    source/Utility/Standard.h
    source/Utility/Console.c
    source/Utility/Console.h
    source/Utility/Dynamic.c
    source/Utility/Dynamic.h
    source/Utility/Panic.c
    source/Utility/Panic.h
    source/Utility/Performance.c
    source/Utility/Performance.h
    source/Utility/UCD.c
    source/Utility/Unicode.c
    source/Utility/Unicode.h
    source/Utility/String.c
    source/Utility/String.h
    source/Utility/File.c
    source/Utility/File.h
    
    source/Structures/Map.c
    source/Structures/Map.h
    source/Structures/Stack.c
    source/Structures/Stack.h
)
