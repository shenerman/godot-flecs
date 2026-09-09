/**************************************************************************/
/*  bridge/input.hpp                                                      */
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

#include "bridge/input.hpp"

#include "logic/input.hpp"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <array>

#include "flecs.h"

namespace bridge {
namespace {

// 动作名集中在此——Input Map 里的配置必须与之一致
constexpr std::array<const char *, 4> MOVE_ACTIONS{
	"move_left",
	"move_right",
	"move_down",
	"move_up",
};

} // namespace

void register_input(flecs::world &p_w) {
	// 动作缺失是配置错误：启动期报一次，不在运行期刷屏
	//（沿用 storm 巡检的"配置错误启动期报"模式）
	for (const char *action : MOVE_ACTIONS) {
		if (!godot::InputMap::get_singleton()->has_action(action)) {
			godot::UtilityFunctions::push_error(godot::vformat(
					"input: Input Map 缺少动作 '%s'——移动输入将失效",
					godot::String(action)));
		}
	}

	// 单例预置：move_by_input 的 has<InputState>() 检查从此恒真，
	// headless 测试直接改写此单例即可驱动移动
	p_w.set<logic::InputState>({});
}

void poll_input(flecs::world &p_w) {
	// get_vector(负x, 正x, 负y, 正y)：此处约定"上 = +y → 逻辑 +z"，
	// 想反转前后方向就对调 move_down / move_up 的位置
	const godot::Vector2 v = godot::Input::get_singleton()->get_vector(
			"move_left", "move_right", "move_down", "move_up");

	p_w.set<logic::InputState>({ .move_x = v.x, .move_y = v.y });
}

} // namespace bridge
