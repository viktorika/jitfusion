cmake_minimum_required(VERSION 3.10)
project(jitfusion VERSION 1.2.1 LANGUAGES CXX)

# GNUInstallDirs defines CMAKE_INSTALL_{LIBDIR,INCLUDEDIR,BINDIR,DOCDIR,...}
# so we need it before any target_include_directories(... $<INSTALL_INTERFACE:...> ...)
# reference below.
include(GNUInstallDirs)

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
target_include_directories(jitfusion
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        src)
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
target_include_directories(athena PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/athena>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
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
    file(GLOB ENGINE_BENCHMARK_SRC "benchmark/*.cc" "benchmark/*.h")
    add_executable(engine_benchmark ${ENGINE_BENCHMARK_SRC})
    target_link_libraries(engine_benchmark jitfusion benchmark::benchmark)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING
            "BUILD_BENCHMARK=ON with CMAKE_BUILD_TYPE=Debug: results will be "
            "meaningless. Reconfigure with -DCMAKE_BUILD_TYPE=Release.")
    endif()
endif()

# -----------------------------------------------------------------------------
# Install / export
#
# `cmake --install <build_dir> --prefix <prefix>` will produce:
#   <prefix>/lib/libjitfusion.{a,so}
#   <prefix>/lib/libathena.{a,so}
#   <prefix>/include/jitfusion/*.h
#   <prefix>/include/athena/*.{h,hh}
#   <prefix>/lib/cmake/jitfusion/jitfusion{Config,ConfigVersion,Targets}.cmake
#
# Downstream usage:
#   find_package(jitfusion 1.2 REQUIRED)
#   target_link_libraries(my_app PRIVATE jitfusion::jitfusion jitfusion::athena)
#
# Set JITFUSION_INSTALL=OFF if you are embedding jitfusion via
# add_subdirectory() from a larger project and do not want install rules.
# -----------------------------------------------------------------------------
option(JITFUSION_INSTALL "Generate install rules for jitfusion" ON)

if(JITFUSION_INSTALL)
    include(CMakePackageConfigHelpers)

    set(JITFUSION_CMAKE_CONFIG_INSTALL_DIR
        "${CMAKE_INSTALL_LIBDIR}/cmake/jitfusion")

    install(TARGETS jitfusion athena
        EXPORT jitfusionTargets
        ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}")

    # jitfusion public headers -> <prefix>/include/jitfusion/
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/jitfusion"
        FILES_MATCHING PATTERN "*.h")

    # athena public headers -> <prefix>/include/athena/
    # Only the files referenced by athena/athena.h (directly or transitively)
    # are part of the public surface; implementation headers like token.ll
    # or the generated shell script are excluded.
    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/athena/athena.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/athena/ast_builder.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/athena/dsu.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/athena/pipeline_grouper.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/athena/location.hh"
            "${CMAKE_CURRENT_SOURCE_DIR}/athena/parser.hh"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/athena")

    # Export the targets so downstream projects pick them up via find_package.
    install(EXPORT jitfusionTargets
        FILE jitfusionTargets.cmake
        NAMESPACE jitfusion::
        DESTINATION "${JITFUSION_CMAKE_CONFIG_INSTALL_DIR}")

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/jitfusionConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    # Generate the Config file from an inline template. `@PACKAGE_INIT@` is
    # expanded by configure_package_config_file() and provides a minimal
    # prelude that makes relative paths inside the config file relocatable.
    set(JITFUSION_CONFIG_TEMPLATE
        "${CMAKE_CURRENT_BINARY_DIR}/jitfusionConfig.cmake.in")
    file(WRITE "${JITFUSION_CONFIG_TEMPLATE}"
[=[@PACKAGE_INIT@

# jitfusion links against LLVM libraries reported by `llvm-config --libs` at
# build time. Those library references are baked into jitfusionTargets.cmake
# as INTERFACE_LINK_LIBRARIES; downstream projects must ensure the matching
# LLVM installation is discoverable by the linker (typically by having
# llvm-config on PATH or by adding -L<llvm-prefix>/lib). jitfusion does not
# call find_package(LLVM) here to avoid forcing CMake-mode LLVM discovery on
# consumers that already link against LLVM through other means.

include("${CMAKE_CURRENT_LIST_DIR}/jitfusionTargets.cmake")
check_required_components(jitfusion)
]=])

    configure_package_config_file(
        "${JITFUSION_CONFIG_TEMPLATE}"
        "${CMAKE_CURRENT_BINARY_DIR}/jitfusionConfig.cmake"
        INSTALL_DESTINATION "${JITFUSION_CMAKE_CONFIG_INSTALL_DIR}")

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/jitfusionConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/jitfusionConfigVersion.cmake"
        DESTINATION "${JITFUSION_CMAKE_CONFIG_INSTALL_DIR}")

    install(FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
            "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        OPTIONAL)
endif()
