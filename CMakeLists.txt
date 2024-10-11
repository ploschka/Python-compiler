cmake_minimum_required(VERSION 3.20)
project(Python_compiler
    VERSION 0.5.0
    DESCRIPTION "Python language subset compiler"
    LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PYCOM_BUILD_TESTS "Build pycom tests" ON)
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
set(codegenlib src/pycom/CodeGenerator.cpp src/pycom/CodeEmittingNodeVisitor.cpp src/pycom/GlobalNodeVisitor.cpp)
set(optimizerlib src/pycom/Optimizer.cpp src/pycom/CalculateVisitor.cpp src/pycom/ShortenExpressionVisitor.cpp)
set(utilitylib src/pycom/ErrorManager.cpp)
set(stdlib src/pycom/stdlib.cpp)
set(runlib src/pycom/run.cpp)

add_library(token STATIC ${tokenlib})
add_library(ast STATIC ${astlib})
add_library(lexer STATIC ${lexlib})
add_library(parser STATIC ${parslib})
add_library(semanalyzer STATIC ${semanalyzerlib})
add_library(codegen STATIC ${codegenlib})
add_library(optimizer STATIC ${optimizerlib})
add_library(utility STATIC ${utilitylib})
add_library(std STATIC ${stdlib})
add_library(run STATIC ${runlib})

target_link_libraries(lexer token)
target_link_libraries(parser lexer ast token)
target_link_libraries(codegen ${llvm_libraries})
target_link_libraries(optimizer ast token)

set(libs lexer parser codegen semanalyzer utility optimizer)

add_library(${libname} SHARED src/pycom/Pycom.cpp)

set(project_targets ${libs} ${libname})

target_link_libraries(${libname} ${libs} ${llvm_libraries} ${targets} ${target_asmparsers})

if(PYCOM_BUILD_EXECUTABLE)
    add_executable(${exename} main.cpp)
    list(APPEND project_targets  ${exename})

    target_link_libraries(${exename} ${libname})
endif(PYCOM_BUILD_EXECUTABLE)

foreach(t ${project_targets})
    if(MSVC)
        target_compile_options(${t} PRIVATE /W4 /WX)
    else()
        target_compile_options(${t} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()       

    set_property(TARGET ${t} PROPERTY CXX_STANDARD 20)
endforeach()

if(PYCOM_BUILD_TESTS)
    set(script_source scripts)
    file(COPY ${script_source} DESTINATION .)
    include_directories(examples)
    add_library(test_visitors STATIC tests/NodePrintVisitor.cpp tests/NodeToVectorVisitor.cpp)
    add_library(throw_errmng STATIC tests/ThrowErrorManager.cpp)

    add_subdirectory(tests/googletest)

    add_library(lexermock STATIC tests/LexerMock.cpp)
    target_link_libraries(lexermock token)

    add_executable(${testname} tests/parsertests.cpp tests/lexertests.cpp tests/semantictests.cpp)

    target_link_libraries(${testname} gtest gtest_main ${libs} throw_errmng test_visitors lexermock)
endif(PYCOM_BUILD_TESTS)

