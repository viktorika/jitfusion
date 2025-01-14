cc_library(
    name = "jitfusion",
    srcs = glob(["src/exec_node/const_value_node.cc"]),
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = [
        "@llvm//:llvm"
    ],
    copts = [
        "-Wall",
        "-Wextra",
        "-Wno-float-equal",
        # "-Werror",
        "-std=c++17",
        #"-mavx2",
        "-O2",
        "-DNDEBUG",
        "-ftree-vectorize",
        "-DLLVM_ENABLE_ABI_BREAKING_CHECKS=0",
    ],
)