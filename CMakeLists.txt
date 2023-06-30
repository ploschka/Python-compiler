cmake_minimum_required(VERSION 3.8)
project(Python_compiler
    VERSION 0.2.2
    DESCRIPTION "Python compiler made using LLVM"
    LANGUAGES C CXX)

option(PYCOM_BUILD_TESTS "Build pycom tests" ON)
option(PYCOM_BUILD_EXAMPLES "Build pycom examples" ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
foreach(target ${LLVM_TARGETS_TO_BUILD})
  list(APPEND targets "LLVM${target}CodeGen")
endforeach()
foreach(target ${LLVM_TARGETS_TO_BUILD})
  list(APPEND target_asmparsers "LLVM${target}AsmParser")
endforeach()
llvm_map_components_to_libnames(llvm_libraries analysis support core object irreader executionengine scalaropts instcombine orcjit runtimedyld)

include_directories(include)

set(exename pycom)
set(testname test_pycom)
set(parsertestname test_parser)

set(tokenlib src/pycom/Token.cpp)
set(astlib src/pycom/ASTNode.cpp src/pycom/AST.cpp)
set(lexlib src/pycom/Lexer.cpp src/pycom/LexerFactory.cpp src/pycom/LexerStates.cpp src/pycom/FileData.cpp)
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
target_link_libraries(parser lexer ast token)
target_link_libraries(codegen ${llvm_libraries})

set(libs lexer parser codegen semanalyzer)

add_executable(${exename} main.cpp)
set_property(TARGET ${exename} PROPERTY CXX_STANDARD 17)
set_property(TARGET codegen PROPERTY CXX_STANDARD 17)

if(MSVC)
  target_compile_options(${exename} PRIVATE /W4 /WX)
else()
  target_compile_options(${exename} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(${exename} ${libs} ${llvm_libraries} ${targets} ${target_asmparsers})

if(PYCOM_BUILD_TESTS OR PYCOM_BUILD_EXAMPLES)
  set(script_source scripts)
  file(COPY ${script_source} DESTINATION .)
endif()

if(PYCOM_BUILD_TESTS)
    add_subdirectory(tests/googletest)
    add_executable(${testname} tests/test.cpp tests/parsertests.cpp src/pycom/NodeToVectorVisitor.cpp)

    target_link_libraries(${testname} gtest gtest_main ${libs})
endif(PYCOM_BUILD_TESTS)

if(PYCOM_BUILD_EXAMPLES)
    set(lexex lexerexample)
    set(parsex parserexample)
    set(examples ${lexex} ${parsex})

    add_library(printast src/pycom/NodePrintVisitor.cpp)

    add_executable(${lexex} examples/lexer.cpp)
    add_executable(${parsex} examples/parser.cpp)

    target_link_libraries(${parsex} printast)

    foreach(ex ${examples})
        target_link_libraries(${ex} ${libs})        
    endforeach()
endif()
