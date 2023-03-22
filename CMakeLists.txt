cmake_minimum_required(VERSION 3.0)
project(Python_compiler, CXX)

add_subdirectory(Lexeme)
add_subdirectory(Lexer)

add_executable(main main.cpp)

