cmake_minimum_required(VERSION 3.8)
project(Python_compiler
    VERSION 0.1.0
    DESCRIPTION "Python compiler made using LLVM"
    LANGUAGES CXX)

option(PYCOM_BUILD_TESTS "Build pycom tests" OFF)

include_directories(include)

set(exename pycom)
set(testname test_pycom)

set(lexemelib src/pycom/Lexeme.cpp)
set(lexlib src/pycom/Lexer.cpp src/pycom/LexerFactory.cpp)
set(parslib src/pycom/Parser.cpp src/pycom/ParserFactory.cpp)
set(codegenlib src/pycom/CodeGenerator.cpp src/pycom/CodeGeneratorFactory.cpp src/pycom/CodeEmittingNodeVisitor.cpp)
set(semanalyzerlib src/pycom/SemanticAnalyzer.cpp src/pycom/SemanticAnalyzerFactory.cpp)

add_library(lexeme STATIC ${lexemelib})
add_library(lexer STATIC ${lexlib})
add_library(parser STATIC ${parslib})
add_library(codegen STATIC ${codegenlib})
add_library(semanalyzer STATIC ${semanalyzerlib})

set(libs lexeme lexer parser codegen semanalyzer)

add_executable(${exename} main.cpp)
set_property(TARGET ${exename} PROPERTY CXX_STANDARD 17)
target_link_libraries(${exename} ${libs})

if(PYCOM_BUILD_TESTS)
    add_subdirectory(tests/googletest)
    add_executable(${testname} tests/test.cpp)
    target_link_libraries(${testname} gtest gtest_main ${libs})
endif()
