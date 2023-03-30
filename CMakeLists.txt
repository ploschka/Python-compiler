cmake_minimum_required(VERSION 3.0)
project(Python_compiler
    VERSION 0.1.0
    DESCRIPTION "Python compiler made using LLVM"
    LANGUAGES CXX)

include_directories(include)

set(exename pycom)

set(lexlib src/pycom/Lexer.cpp src/pycom/Lexeme.cpp src/pycom/LexerFactory.cpp)
set(parslib src/pycom/Parser.cpp src/pycom/ParserFactory.cpp)
set(codegenlib src/pycom/CodeGenerator.cpp src/pycom/CodeGeneratorFactory.cpp src/pycom/CodeEmittingNodeVisitor.cpp)
set(semanalyzerlib src/pycom/SemanticAnalyzer.cpp src/pycom/SemanticAnalyzerFactory.cpp)

add_library(lexer STATIC ${lexlib})
add_library(parser STATIC ${parslib})
add_library(codegen STATIC ${codegenlib})
add_library(semanalyzer STATIC ${semanalyzerlib})

set(libs lexer parser codegen semanalyzer)

# add_subdirectory(tests/googletest)

add_executable(${exename} main.cpp)
target_link_libraries(${exename} ${libs})
