workspace(name = "jitfusion")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//:llvm_repository.bzl", "llvm_repository")

http_archive(
    name = "bazel_compdb",
    strip_prefix = "bazel-compilation-database-master",
    urls = ["https://github.com/grailbio/bazel-compilation-database/archive/master.zip"],
)

# new_local_repository(
#     name = "zstd",
#     path = "/opt/homebrew/Cellar/zstd/1.5.6",
#     build_file_content = """
# cc_library(
#     name = "zstd",
#     hdrs = glob(["include/**"]),
#     srcs = glob(["lib/*.a"]),
#     includes = ["include"],
#     visibility = ["//visibility:public"],
# )
# """
# )

llvm_repository(
    name = "llvm",
)