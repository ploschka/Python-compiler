cmake_minimum_required(VERSION 3.8)
project(Python_compiler
    VERSION 0.1.0
    DESCRIPTION "Python compiler made using LLVM"
    LANGUAGES CXX)

option(PYCOM_BUILD_TESTS "Build pycom tests" OFF)
option(PYCOM_BUILD_EXAMPLES "Build pycom examples" ON)

include_directories(include)

set(exename pycom)
set(testname test_pycom)

set(tokenlib src/pycom/Token.cpp)
set(astlib src/pycom/ASTNode.cpp src/pycom/AST.cpp)
set(lexlib src/pycom/Lexer.cpp src/pycom/LexerFactory.cpp src/pycom/LexerStates.cpp)
set(parslib src/pycom/Parser.cpp src/pycom/ParserFactory.cpp)
set(semanalyzerlib src/pycom/SemanticAnalyzer.cpp src/pycom/SemanticAnalyzerFactory.cpp)
set(codegenlib src/pycom/CodeGenerator.cpp src/pycom/CodeGeneratorFactory.cpp src/pycom/CodeEmittingNodeVisitor.cpp)

add_library(token STATIC ${tokenlib})
add_library(ast STATIC ${astlib})
add_library(lexer STATIC ${lexlib})
add_library(parser STATIC ${parslib})
add_library(semanalyzer STATIC ${semanalyzerlib})
add_library(codegen STATIC ${codegenlib})

target_link_libraries(lexer token)

set(libs lexer parser codegen semanalyzer)

add_executable(${exename} main.cpp)
set_property(TARGET ${exename} PROPERTY CXX_STANDARD 17)
target_link_libraries(${exename} ${libs})

if(PYCOM_BUILD_TESTS)
    add_subdirectory(tests/googletest)
    add_executable(${testname} tests/test.cpp)
    target_link_libraries(${testname} gtest gtest_main ${libs})
endif(PYCOM_BUILD_TESTS)

if(PYCOM_BUILD_EXAMPLES)
    set(script_source examples/scripts)
    set(script_output scripts)

    set(lexex lexerexample)
    set(examples ${lexex})
    set(example_scripts lexer.py)

    add_executable(${lexex} examples/lexer.cpp)

    foreach(ex ${examples})
        target_link_libraries(${ex} ${libs})        
    endforeach()

    foreach(ex ${example_scripts})
        configure_file(${script_source}/${ex} ${script_output}/${ex} COPYONLY)        
    endforeach()    
endif()
