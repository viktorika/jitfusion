cc_library(
    name = "jitfusion",
    srcs = glob(["src/**/*.cc"] + ["src/**/*.h"]),
    hdrs = glob(["include/*.h"]),
    includes = ["include", "src"],
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
        #"-g",
        #"-Rpass=loop-vectorize",
        "-DHAS_XSIMD",
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

cc_library(
    name = "athena",
    srcs = [
        "athena/athena.cc",
        "athena/ast_builder.cc",
        "athena/ast_builder.h",
        "athena/location.hh",
        "athena/parser.cc",
        "athena/parser.hh",
        "athena/token.cc",
    ],
    hdrs = [
        "athena/athena.h",
    ],
    includes = [
        "athena",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:jitfusion",
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
        #"-g",
    ],
)

cc_binary(
    name = "athena_test",
    srcs = ["athena/athena_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":athena"
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