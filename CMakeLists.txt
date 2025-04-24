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

# 添加编译选项
add_compile_options(
    -Wall
    -Wextra
    -Wno-float-equal
    -O2
    -DNDEBUG
    -ftree-vectorize
    -DLLVM_ENABLE_ABI_BREAKING_CHECKS=0
)

file(GLOB_RECURSE JITFUSION_SRC "src/*.cc" "src/*.h")
file(GLOB JITFUSION_HDRS "include/*.h")

add_library(jitfusion ${JITFUSION_SRC} ${JITFUSION_HDRS})
target_include_directories(jitfusion PUBLIC include PRIVATE src)
target_link_libraries(jitfusion ${LLVM_LIBS})

# 添加可执行文件
file(GLOB_RECURSE JITFUSION_TEST_SRC "test/*.cc")

# 查找 GTest 包
find_package(GTest REQUIRED)

add_executable(jitfusion_test ${JITFUSION_TEST_SRC})
target_link_libraries(jitfusion_test jitfusion GTest::GTest)
