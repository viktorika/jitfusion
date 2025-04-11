cc_library(
    name = "athena",
    srcs = [
        "athena.cc",
        "ast_builder.cc",
        "ast_builder.h",
        "location.hh",
        "parser.cc",
        "parser.hh",
        "token.cc",
    ],
    hdrs = [
        "athena.h",
    ],
    includes = [
        ".",
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
    ],
)

cc_binary(
    name = "athena_test",
    srcs = ["athena_test.cc"],
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