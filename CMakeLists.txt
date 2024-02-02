cmake_minimum_required(VERSION 3.13)
project(Python_compiler
    VERSION 0.5.0
    DESCRIPTION "Python compiler made using LLVM"
    LANGUAGES C CXX)

option(PYCOM_BUILD_TESTS "Build pycom tests" ON)
option(PYCOM_BUILD_EXAMPLES "Build pycom examples" ON)
option(PYCOM_BUILD_EXECUTABLE "Build pycom executable" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
foreach(target ${LLVM_TARGETS_TO_BUILD})
  list(APPEND targets "LLVM${target}CodeGen")
endforeach()
# foreach(target ${LLVM_TARGETS_TO_BUILD})
#   list(APPEND target_asmparsers "LLVM${target}AsmParser")
# endforeach()
list(APPEND target_asmparsers LLVMX86AsmParser)

llvm_map_components_to_libnames(llvm_libraries analysis support core object irreader executionengine scalaropts instcombine orcjit runtimedyld)

include_directories(include)

set(exename pycom)
set(testname test_pycom)
set(libname pycomlib)

set(tokenlib src/pycom/Token.cpp)
set(astlib src/pycom/ASTNode.cpp src/pycom/AST.cpp)
set(lexlib src/pycom/Lexer.cpp src/pycom/LexerStates.cpp src/pycom/LexerData.cpp)
set(parslib src/pycom/Parser.cpp)
set(semanalyzerlib src/pycom/SemanticAnalyzer.cpp src/pycom/SemanticVisitor.cpp)
set(codegenlib src/pycom/CodeGenerator.cpp src/pycom/CodeEmittingNodeVisitor.cpp)
set(optimizerlib src/pycom/Optimizer.cpp src/pycom/CalculateVisitor.cpp src/pycom/ShortenExpressionVisitor.cpp)
set(utilitylib src/pycom/ErrorManager.cpp)

add_library(token STATIC ${tokenlib})
add_library(ast STATIC ${astlib})
add_library(lexer STATIC ${lexlib})
add_library(parser STATIC ${parslib})
add_library(semanalyzer STATIC ${semanalyzerlib})
add_library(codegen STATIC ${codegenlib})
add_library(optimizer STATIC ${optimizerlib})
add_library(utility STATIC ${utilitylib})

target_link_libraries(lexer token)
target_link_libraries(parser lexer ast token)
target_link_libraries(codegen ${llvm_libraries})
target_link_libraries(optimizer ast token)

set(libs lexer parser codegen semanalyzer utility optimizer)

add_library(${libname} SHARED src/pycom/Pycom.cpp)
set_property(TARGET codegen PROPERTY CXX_STANDARD 17)

foreach(l ${libs} ${libname})
  if(MSVC)
    target_compile_options(${l} PRIVATE /W4 /WX)
  else()
    target_compile_options(${l} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()       
endforeach()

target_link_libraries(${libname} ${libs} ${llvm_libraries} ${targets} ${target_asmparsers})

if(PYCOM_BUILD_EXECUTABLE)
  add_executable(${exename} main.cpp)
  set_property(TARGET ${exename} PROPERTY CXX_STANDARD 17)

  if(MSVC)
    target_compile_options(${exename} PRIVATE /W4 /WX)
  else()
    target_compile_options(${exename} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()

  target_link_libraries(${exename} ${libname})
endif(PYCOM_BUILD_EXECUTABLE)

if(PYCOM_BUILD_TESTS OR PYCOM_BUILD_EXAMPLES)
  set(script_source scripts)
  file(COPY ${script_source} DESTINATION .)
  include_directories(examples)
  add_library(ex_visitors examples/NodePrintVisitor.cpp examples/NodeToVectorVisitor.cpp)
endif()

if(PYCOM_BUILD_TESTS)
    add_subdirectory(tests/googletest)

    add_library(lexermock STATIC tests/LexerMock.cpp)
    add_library(throwem STATIC tests/ThrowErrorManager.cpp)
    target_link_libraries(lexermock token)

    add_executable(${testname} tests/parsertests.cpp tests/lexertests.cpp tests/semantictests.cpp)

    target_link_libraries(${testname} gtest gtest_main ${libs} ex_visitors lexermock throwem)
endif(PYCOM_BUILD_TESTS)

if(PYCOM_BUILD_EXAMPLES)
    set(lexex lexerexample)
    set(parsex parserexample)
    set(optimex optimizerexample)
    set(examples ${lexex} ${parsex} ${optimex})

    add_executable(${lexex} examples/lexer.cpp)
    add_executable(${parsex} examples/parser.cpp)
    add_executable(${optimex} examples/optimizer.cpp)

    target_link_libraries(${parsex} ex_visitors)
    target_link_libraries(${optimex} ex_visitors)

    foreach(ex ${examples})
        target_link_libraries(${ex} ${libs})        
    endforeach()
endif()
