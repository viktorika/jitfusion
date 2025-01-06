def _llvm_repository_impl(repository_ctx):
    # 获取 llvm-config 的路径
    llvm_config_path = repository_ctx.which("llvm-config")
    if not llvm_config_path:
        fail("Could not find llvm-config in PATH")

    # 执行 llvm-config --prefix 获取 LLVM 的安装路径
    result = repository_ctx.execute([llvm_config_path, "--prefix"])
    if result.return_code != 0:
        fail("Failed to execute llvm-config --prefix: " + result.stderr)

    llvm_prefix = result.stdout.strip()

    # 生成 BUILD 文件内容
    build_file_content = """
cc_library(
    name = "llvm",
    hdrs = glob(["llvm/include/**"]),
    srcs = glob(["llvm/lib/*.a"]),
    includes = ["llvm/include"],
    visibility = ["//visibility:public"],
    linkopts = ["-lpthread", "-lz", "-ldl", "-lzstd"],
    #deps=["@zstd//:zstd"],
)
"""

    # 创建 symlink 到 LLVM 的路径
    repository_ctx.symlink(llvm_prefix, "llvm")

    # 创建 BUILD 文件
    repository_ctx.file("BUILD", build_file_content)

llvm_repository = repository_rule(
    implementation = _llvm_repository_impl,
    local = True,
)