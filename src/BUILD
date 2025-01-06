cc_binary(
    name = "test",
    srcs = ["test.cc"],
    includes = ["."],
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
        "-mavx2",
        "-O2",
        "-DNDEBUG",
        "-ftree-vectorize",
        "-DLLVM_ENABLE_ABI_BREAKING_CHECKS=0",
    ],
)