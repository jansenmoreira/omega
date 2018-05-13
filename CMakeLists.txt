cmake_minimum_required(VERSION 3.0)

project(omega)

include_directories(include)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NATVIS:${CMAKE_SOURCE_DIR}/omega.natvis")

find_package(PythonInterp REQUIRED)

add_definitions(-DTEST)

add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/UCD.py --ucd ${CMAKE_SOURCE_DIR}/scripts/ucd.all.flat.zip --output ${PROJECT_BINARY_DIR}/source
    DEPENDS scripts/UCD.py
    OUTPUT  source/Utility/UCD.c
    COMMENT Generating Utility/UCD.c
)

enable_language(ASM_MASM)

add_library(
    omega-utility

    include/Utility/Standard.h
    
    source/Utility/Console.c
    include/Utility/Console.h
    
    source/Utility/Dynamic.c
    include/Utility/Dynamic.h
    
    source/Utility/Panic.c
    include/Utility/Panic.h
    
    source/Utility/Performance.c
    include/Utility/Performance.h

    source/Utility/UCD.c
    include/Utility/Unicode.h
    
    source/Utility/String.c
    include/Utility/String.h
    
    source/Utility/File.c
    include/Utility/File.h
    
    source/Utility/Native_Call.asm
    include/Utility/Native_Call.h
)

add_library(
    omega-structures
    
    source/Structures/Map.c
    include/Structures/Map.h
    
    source/Structures/Stack.c
    include/Structures/Stack.h
)

add_library(
    omega-machine
    
    source/Machine/Execution.c
    include/Machine/Execution.h
    
    source/Machine/Machine.c
    include/Machine/Machine.h
)

add_executable(
    omega-cli

    source/CLI.c
    
    source/Interpreter/Types.c
    include/Interpreter/Types.h
    
    source/Interpreter/Expression.c
    include/Interpreter/Expression.h
    
    source/Interpreter/Lexer.c
    include/Interpreter/Lexer.h
    
    source/Interpreter/Interpreter.c
    include/Interpreter/Interpreter.h
)

target_link_libraries(omega-cli omega-utility omega-structures omega-machine)
