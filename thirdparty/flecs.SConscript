#!/usr/bin/env python
# thirdparty/flecs.SConscript
# pyright: reportUndefinedVariable=false
# flecs 构建与消费配置的唯一归属：
#   - 编译期配置 → flecs_env（Clone，隔离警告/标准/头文件路径）
#   - 消费方配置 → 直接修改传入的主 env（Append 原地生效，顶层可见）
# 产物统一落在顶层 build/thirdparty/flecs/ 下

Import("env")

FLECS_DIST = "#thirdparty/flecs/distr"
FLECS_BUILD = "#build/thirdparty/flecs"

flecs_env = env.Clone()


# thirdparty 隔离：关闭所有警告（参考 Godot methods.py disable_warnings）
def _disable_warnings(e):
    cc = str(e.get("CC", ""))
    if e.get("is_msvc", False) and "clang" not in cc:
        for key in ("CCFLAGS", "CFLAGS", "CXXFLAGS"):
            e[key] = [x for x in e[key] if not (x.startswith("/W") or x.startswith("/w"))]
        e.AppendUnique(CCFLAGS=["/w"])
    else:
        e.AppendUnique(CCFLAGS=["-w"])


_disable_warnings(flecs_env)

# gcc/clang 家族需 gnu99；MSVC 风格工具链（cl/clang-cl）不加
if not flecs_env.get("is_msvc", False):
    flecs_env.Append(CFLAGS=["-std=gnu99"])

# 本模块头文件路径（flecs_env 用；主 env 也 Prepend 一份，供 src/*.cpp 包含 flecs.h）
flecs_env.Prepend(CPPPATH=[FLECS_DIST])

# --- 消费方配置：直接作用于传入的 env，顶层无需重复 ---
env.Prepend(CPPPATH=[FLECS_DIST])

# 对齐 flecs CMakeLists 静态 target 的 PUBLIC 定义：
# 告知 flecs.h 走静态链接的 API 宏分支
env.Append(CPPDEFINES=["flecs_STATIC"])

# LIBS 只在执行链接的环境生效——静态库自身不链接，
# 最终链接发生在主 env 的 SharedLibrary，所以系统库声明在传入的 env 上
if env["platform"] == "windows":
    env.Append(LIBS=["dbghelp", "ws2_32", "wsock32"])
elif env["platform"] == "linux":
    env.Append(LIBS=["pthread", "rt", "m"])

# 先 SharedObject：显式落位 build/（幂等 emitter 不会二次重定向），并保证 PIC
# （静态库链入共享库的前提，对应 CMake 的 FLECS_PIC）
flecs_obj = flecs_env.SharedObject(
    FLECS_BUILD + "/flecs",
    source=FLECS_DIST + "/flecs.c",
)

# StaticLibrary 只做归档、不链接，必须显式指定输出
libflecs = flecs_env.StaticLibrary(FLECS_BUILD + "/flecs", source=[flecs_obj])

Return("libflecs")
