/**************************************************************************/
/*  logic/systems.cpp                                                    */
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

#include "logic/components.hpp"
#include "logic/input.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <flecs.h>
#include <random>

namespace logic {
namespace {

float rnd(float p_lo, float p_hi) {
	// 固定种子是刻意的：T5 弹幕验收需要可复现的出生序列
	// NOLINTNEXTLINE(cert-msc51-cpp)
	static std::mt19937 rng{ 42U }; // NOLINT(cert-msc32-c, cert-msc51-cpp)
	return std::uniform_real_distribution<float>(p_lo, p_hi)(rng);
}

} // namespace

void register_systems(flecs::world &p_w) {
	p_w.system<const MoveSpeed, Position>("move_by_input")
			.kind(flecs::OnUpdate)
			.each([](flecs::iter &p_it, size_t /*p_i*/,
						  const MoveSpeed &p_speed, Position &p_pos) {
				if (!p_it.world().has<InputState>()) {
					return;
				}
				const auto &in = p_it.world().get<InputState>();

				const float len = std::sqrt(
						(in.move_x * in.move_x) + (in.move_y * in.move_y));
				if (len <= 0.0001F) {
					return; // 没推摇杆，省掉归一化的除零
				}
				const float nx = in.move_x / std::max(len, 1.0F);
				const float ny = in.move_y / std::max(len, 1.0F);
				p_pos.x += (nx * p_speed.value) * p_it.delta_time();
				p_pos.z += (ny * p_speed.value) * p_it.delta_time();
			});

	p_w.system<const TestInput>("test_spawn_bullets")
			.kind(flecs::OnUpdate)
			.run([](flecs::iter &p_it) {
				flecs::world w = p_it.world();
				if (!w.has<TestInput>() || !w.get<TestInput>().storm) {
					return;
				}
				flecs::entity tmpl = w.lookup("bullet");
				if (!tmpl.is_valid()) {
					w.get_mut<TestInput>().storm = false;
					return;
				}
				for (int k = 0; k < 30; ++k) {
					w.entity()
							.set<SpawnRequest>({
									.identity = tmpl,
									.position =
											Position{ rnd(-5.0F, 5.0F),
													rnd(2.0F, 6.0F),
													rnd(-5.0F, 5.0F) },
									.rotation = {},
									.scale = {},
							});
				}
			});

	p_w.system<Life>("expire")
			.kind(flecs::OnUpdate)
			.each([](flecs::iter &p_it, size_t p_i, Life &p_life) {
				p_life.t -= p_it.delta_time();
				if (p_life.t <= 0.0F) {
					p_it.entity(p_i).add<Despawn>();
				}
			});
}

} // namespace logic