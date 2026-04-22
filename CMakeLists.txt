cmake_minimum_required(VERSION 3.10)
project(jitfusion_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(LLVM_CONFIG llvm-config)
if(NOT LLVM_CONFIG)
    message(FATAL_ERROR "Could not find llvm-config in PATH")
endif()

execute_process(
    COMMAND ${LLVM_CONFIG} --prefix
    OUTPUT_VARIABLE LLVM_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG} --libs
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(${LLVM_PREFIX}/include)
link_directories(${LLVM_PREFIX}/lib)

add_compile_options(
    -Wall
    -Wextra
    -Wno-float-equal
    -O3
    -DNDEBUG
    -ftree-vectorize
)

# Sanitizer support: -DSANITIZER=address|undefined|thread
if(DEFINED SANITIZER)
    message(STATUS "Building with sanitizer: ${SANITIZER}")
    if(SANITIZER STREQUAL "address")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    elseif(SANITIZER STREQUAL "undefined")
        add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=undefined)
    elseif(SANITIZER STREQUAL "address,undefined")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    elseif(SANITIZER STREQUAL "thread")
        add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
        add_link_options(-fsanitize=thread)
    else()
        message(FATAL_ERROR "Unknown sanitizer: ${SANITIZER}. Use address, undefined, address,undefined, or thread.")
    endif()
endif()

# Coverage support: -DCOVERAGE=ON
if(COVERAGE)
    message(STATUS "Building with coverage instrumentation")
    add_compile_options(--coverage -fno-inline -O0)
    add_link_options(--coverage)
endif()

if(NOT DEFINED HAS_XSIMD)
    message(STATUS "default set HAS_XSIMD to ON")
    set(HAS_XSIMD ON)
endif()

if(DEFINED HAS_XSIMD)
    message(STATUS "compile with xsimd")
    add_compile_definitions(HAS_XSIMD)
endif()

file(GLOB_RECURSE JITFUSION_SRC "src/*.cc" "src/*.h")
file(GLOB JITFUSION_HDRS "include/*.h")

add_library(jitfusion ${JITFUSION_SRC} ${JITFUSION_HDRS})
target_include_directories(jitfusion PUBLIC include PRIVATE src)
target_link_libraries(jitfusion ${LLVM_LIBS})

file(GLOB_RECURSE JITFUSION_TEST_SRC "test/*.cc")

find_package(GTest REQUIRED)

add_executable(jitfusion_test ${JITFUSION_TEST_SRC})
target_link_libraries(jitfusion_test jitfusion GTest::GTest GTest::Main)

file(GLOB_RECURSE ATHENA_SRC 
    "athena/athena.cc"
    "athena/ast_builder.cc"
    "athena/ast_builder.h"
    "athena/location.hh"
    "athena/parser.cc"
    "athena/parser.hh"
    "athena/token.cc"
    "athena/dsu.cc"
    "athena/dsu.h"
    "athena/pipeline_grouper.cc"
    "athena/pipeline_grouper.h"
)

add_library(athena ${ATHENA_SRC})
target_include_directories(jitfusion PUBLIC athena)
target_link_libraries(athena jitfusion)

file(GLOB_RECURSE ATHENA_TEST_SRC "athena/test/*.cc")

add_executable(athena_test ${ATHENA_TEST_SRC})
target_include_directories(athena_test PRIVATE athena/test)
target_link_libraries(athena_test athena GTest::GTest GTest::Main)
