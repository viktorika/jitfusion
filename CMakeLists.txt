cmake_minimum_required(VERSION 3.10)
project(jitfusion_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    message(STATUS "Building with coverage instrumentation (forcing CMAKE_BUILD_TYPE=Debug)")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Forced to Debug for coverage" FORCE)
    add_compile_options(--coverage -fno-inline)
    add_link_options(--coverage)
endif()

# xsimd support.
#
# HAS_XSIMD accepts three values:
#   ON   : xsimd is required; configure fails if the headers are not found.
#   OFF  : xsimd is disabled; the fallback scalar code path is used.
#   AUTO : try to detect xsimd and silently fall back to OFF if not available.
#          This is the default so a fresh clone still builds on machines that
#          do not have xsimd installed, while still picking it up when present.
#
# Detection is done here in the configure step rather than being deferred to
# the first #include <xsimd/xsimd.hpp> so that a misconfiguration is surfaced
# immediately and with a meaningful error message.
set(HAS_XSIMD "AUTO" CACHE STRING "Enable xsimd acceleration (ON / OFF / AUTO)")
set_property(CACHE HAS_XSIMD PROPERTY STRINGS "ON" "OFF" "AUTO")

set(JITFUSION_USE_XSIMD OFF)
set(JITFUSION_XSIMD_INCLUDE_DIRS "")

if(NOT HAS_XSIMD STREQUAL "OFF")
    # Prefer the upstream CMake package when it is installed (xsimd >= 9 ships
    # xsimdConfig.cmake), otherwise fall back to a header-only lookup. Using
    # QUIET here so AUTO stays silent on failure.
    find_package(xsimd CONFIG QUIET)
    if(xsimd_FOUND)
        set(JITFUSION_USE_XSIMD ON)
        # Config-mode package exposes the imported target xsimd::xsimd; we
        # therefore do not need to set include dirs by hand.
        message(STATUS "xsimd: found via CMake config package (version ${xsimd_VERSION})")
    else()
        find_path(JITFUSION_XSIMD_INCLUDE_DIR
            NAMES xsimd/xsimd.hpp
            DOC "Directory that contains xsimd/xsimd.hpp")
        if(JITFUSION_XSIMD_INCLUDE_DIR)
            set(JITFUSION_USE_XSIMD ON)
            set(JITFUSION_XSIMD_INCLUDE_DIRS "${JITFUSION_XSIMD_INCLUDE_DIR}")
            message(STATUS "xsimd: found headers at ${JITFUSION_XSIMD_INCLUDE_DIR}")
        endif()
    endif()

    if(NOT JITFUSION_USE_XSIMD)
        if(HAS_XSIMD STREQUAL "ON")
            message(FATAL_ERROR
                "HAS_XSIMD=ON was requested but xsimd headers could not be found. "
                "Either install xsimd (see README) or reconfigure with "
                "-DHAS_XSIMD=OFF / -DHAS_XSIMD=AUTO.")
        else()
            message(STATUS "xsimd: not found, falling back to scalar code path (HAS_XSIMD=AUTO)")
        endif()
    endif()
endif()

if(JITFUSION_USE_XSIMD)
    message(STATUS "compile with xsimd")
else()
    message(STATUS "compile without xsimd")
endif()

file(GLOB_RECURSE JITFUSION_SRC "src/*.cc" "src/*.h")
file(GLOB JITFUSION_HDRS "include/*.h")

add_library(jitfusion ${JITFUSION_SRC} ${JITFUSION_HDRS})
target_include_directories(jitfusion PUBLIC include PRIVATE src)
target_link_libraries(jitfusion ${LLVM_LIBS})

# xsimd is only used by src/function/list_arithmetic.cc and never leaks into
# any public header under include/, so PRIVATE is the correct visibility here.
# This keeps the athena / test / benchmark targets free of the xsimd include
# path and the HAS_XSIMD macro.
if(JITFUSION_USE_XSIMD)
    target_compile_definitions(jitfusion PRIVATE HAS_XSIMD)
    if(TARGET xsimd::xsimd)
        target_link_libraries(jitfusion PRIVATE xsimd::xsimd)
    else()
        target_include_directories(jitfusion PRIVATE ${JITFUSION_XSIMD_INCLUDE_DIRS})
    endif()
endif()

file(GLOB_RECURSE JITFUSION_TEST_SRC "test/*.cc")

option(BUILD_TEST "Build unit tests (requires GTest)" ON)
if(BUILD_TEST)
    find_package(GTest REQUIRED)

    add_executable(jitfusion_test ${JITFUSION_TEST_SRC})
    target_link_libraries(jitfusion_test jitfusion GTest::GTest GTest::Main)
endif()

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
target_include_directories(athena PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/athena)
target_link_libraries(athena jitfusion)

file(GLOB_RECURSE ATHENA_TEST_SRC "athena/test/*.cc")

if(BUILD_TEST)
    add_executable(athena_test ${ATHENA_TEST_SRC})
    target_include_directories(athena_test PRIVATE athena/test)
    target_link_libraries(athena_test athena GTest::GTest GTest::Main)
endif()

option(BUILD_BENCHMARK "Build google-benchmark based micro benchmarks" OFF)
if(BUILD_BENCHMARK)
    find_package(benchmark REQUIRED)
    add_executable(engine_benchmark benchmark/engine_benchmark.cc)
    target_link_libraries(engine_benchmark jitfusion benchmark::benchmark)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING
            "BUILD_BENCHMARK=ON with CMAKE_BUILD_TYPE=Debug: results will be "
            "meaningless. Reconfigure with -DCMAKE_BUILD_TYPE=Release.")
    endif()
endif()
