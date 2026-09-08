/**************************************************************************/
/*  logic/templates.cpp                                                   */
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

#include "logic/templates.hpp"

#include "components.hpp"

namespace logic {

// ── 各种类的装配函数。新增种类 = 写一个 build_<名字> + TEMPLATES 加一行 ──

bool build_bullet(flecs::entity p_e) {
	p_e.add<Bullet>()
			.set<Life>({ 2.0F });
	return true;
}

bool build_enemy(flecs::entity /*p_e*/) {
	// 敌人逻辑尚未设计——占位行，spawn 到它会报错并处置请求
	return false;
}

BuildFn build_from_name(std::string_view p_name) {
	for (const LookupEntry &entry : TEMPLATES) {
		if (entry.name == p_name) {
			return entry.build;
		}
	}
	return nullptr;
}

} // namespace logic
