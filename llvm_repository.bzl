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

    # 和 CMake 保持一致，通过 llvm-config 动态获取所有链接参数
    # --libs: 核心库（如 -lLLVM-19），不会引入包含 main() 的工具入口库
    # --system-libs: 系统依赖库（如 -lpthread -lz 等，动态库模式下通常为空）
    # --ldflags: 链接器选项（如 -L 路径）
    linkopts_list = []

    for flag in ["--libs", "--system-libs", "--ldflags"]:
        result = repository_ctx.execute([llvm_config_path, flag])
        if result.return_code != 0:
            fail("Failed to execute llvm-config %s: %s" % (flag, result.stderr))
        for opt in result.stdout.strip().split(" "):
            opt = opt.strip()
            if opt:
                linkopts_list.append('"%s"' % opt)

    linkopts_str = ", ".join(linkopts_list)

    # 生成 BUILD 文件内容
    build_file_content = """
cc_library(
    name = "llvm",
    hdrs = glob(["llvm/include/**"]),
    includes = ["llvm/include"],
    visibility = ["//visibility:public"],
    linkopts = [{linkopts}],
)
""".format(linkopts = linkopts_str)

    # 创建 symlink 到 LLVM 的路径
    repository_ctx.symlink(llvm_prefix, "llvm")

    # 创建 BUILD 文件
    repository_ctx.file("BUILD", build_file_content)

llvm_repository = repository_rule(
    implementation = _llvm_repository_impl,
    local = True,
)