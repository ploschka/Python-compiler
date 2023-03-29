cmake_minimum_required(VERSION 3.0)
project(Python_compiler
    VERSION 0.1.0
    DESCRIPTION "Python compiler made using LLVM"
    LANGUAGES CXX)

include_directories(include)

add_library(lexer src/pycom/Lexer.cpp)

# add_subdirectory(tests/googletest)

add_executable(main main.cpp)

