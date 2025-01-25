cc_library(
    name = "jitfusion",
    srcs = glob(["src/**/*.cc"] + ["src/**/*.h"]),
    hdrs = glob(["include/*.h"]),
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

cc_binary(
    name = "jitfusion_test",
    srcs = glob(["test/**/*.cc"]),
    visibility = ["//visibility:public"],
    deps = [
        ":jitfusion"
    ],
    copts = [
        "-Wall",
        "-Wextra",
        "-Wno-float-equal",
        # "-Werror",
        "-std=c++17",
        #"-mavx2",
        "-O2",
        "-ftree-vectorize",
    ],
    linkopts = [
        "-lgtest",
    ],
)

cc_binary(
    name = "a",
    srcs = [
        "a.cc",
    ],
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
        "-ftree-vectorize",
    ],
)