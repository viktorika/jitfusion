workspace(name = "jitfusion")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//:llvm_repository.bzl", "llvm_repository")

http_archive(
    name = "bazel_compdb",
    strip_prefix = "bazel-compilation-database-master",
    urls = ["https://github.com/grailbio/bazel-compilation-database/archive/master.zip"],
)

llvm_repository(
    name = "llvm",
)