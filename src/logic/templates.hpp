/**************************************************************************/
/*  logic/templates.hpp                                                   */
/**************************************************************************/
/*                        This file is part of:                           */
/*                             GODOT-FLECS                                */
/*                https://github.com/shenerman/godot-flecs                */
/**************************************************************************/
/* Copyright (c) 2026 shenerman.                                          */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include <array>
#include <flecs.h>
#include <string_view>

namespace logic {

// 装配函数签名：往一个空实例上挂逻辑组件。
// 返回 false = 该种类的逻辑尚未设计（占位行），spawn 侧报错处置
using BuildFn = bool (*)(flecs::entity);

// 装配函数即编译期身份：logic 内部若编译期就知道要哪种，直接调函数；
// 表只服务数据驱动路径（编辑器来的字符串）
bool build_bullet(flecs::entity p_e);
bool build_enemy(flecs::entity p_e);

// 装载后的组件：只带载荷（装配）。名字不进 prefab——
// 它只是查找键，装载期比对完就完成使命
struct LogicTemplate {
	BuildFn build;
};

// 查找表条目：键 + 载荷，名字只活在这里
struct LookupEntry {
	std::string_view name;
	BuildFn build;
};

// 唯一清单：加模板 = 写 build 函数 + 这里加一行。
// 长度与条目数失配会在编译期报错（条目无默认构造，少编不过；多也编不过）
inline constexpr std::array<LookupEntry, 2> TEMPLATES = {
	LookupEntry{ "bullet", &build_bullet },
	LookupEntry{ "enemy", &build_enemy },
};

// 名字 → 装配（装载期调用）。
// nullptr = 设计师填了 logic 侧没有的名字
BuildFn build_from_name(std::string_view p_name);

} // namespace logic
