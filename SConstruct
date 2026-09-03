#!/usr/bin/env python
# pyright: reportUndefinedVariable=false

import os
import sys

from SCons.Builder import ListEmitter  # [MODIFIED] 新增：注册 emitter 时打包 emitter 链

from methods import print_error, redirect_emitter  # [MODIFIED] 改为直接导入 emitter 本体


libname = "godot_flecs"
projectdir = "project"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)

# [MODIFIED] 与 Godot 引擎对齐：opts.Add 位于 Variables() 之后、Update() 之前
opts.Add(BoolVariable(
    "redirect_build_objects",
    "Enable redirecting built objects to `bin/obj/` to declutter the repository.",
    True,
))

opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=["src/"])

# [MODIFIED] BEGIN: 参考 Godot 引擎 SConstruct 末尾的 emitter 注册方式，
# 在 SharedObject 的 emitter 链前插入重定向 emitter，使编译产物进入 bin/obj/。
# 只 patch SharedObject；SharedLibrary 的目标路径（bin/<platform>/）保持显式声明。
for key in (emitters := env.SharedObject.builder.emitter):
    emitters[key] = ListEmitter([redirect_emitter] + env.Flatten(emitters[key]))
# [MODIFIED] END

sources = Glob("src/*.cpp")

if env["target"] in ["editor", "template_debug"]:
    try:
        # [MODIFIED] 生成文件是构建产物，从 src/gen/ 移到根目录 gen/，避免污染源码目录
        doc_data = env.GodotCPPDocData("gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
