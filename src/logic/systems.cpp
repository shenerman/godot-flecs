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

#include "components.hpp"

#include <flecs.h>

#include <random>
#include <godot_cpp/variant/utility_functions.hpp>

#include "bridge/despawn.hpp"

namespace logic {

namespace {

float rnd(float p_lo, float p_hi) {
    // 固定种子是刻意的：T5 弹幕验收需要可复现的出生序列
    // NOLINTNEXTLINE(cert-msc51-cpp)
    static std::mt19937 rng{42U}; // NOLINT(cert-msc32-c, cert-msc51-cpp)
    return std::uniform_real_distribution<float>(p_lo, p_hi)(rng);
}

} // namespace

void register_systems(flecs::world &p_w) {
    // 测试 T5：子弹流。请求实体只挂 SpawnRequest——
    // 标签、寿命等模板数据由 spawner 按模板句柄配给真正的子弹实体。
    p_w.system<const TestInput>("test_spawn_bullets")
        .kind(flecs::OnUpdate)
        .run([](flecs::iter &p_it) {
            flecs::world w = p_it.world();

            if (!w.has<TestInput>() || !w.get<TestInput>().storm) {
                return;
            }

            // 模板句柄：按名字查装载期建好的 prefab 实体。
            // 名字 = template_list.tres 里 bullet 那条的 template_name，
            // 与 load_templates 的 set_name() 同源。风暴系统只认名字，
            // 具体场景/组件全在清单里——换弹种只改 .tres 不改代码
            flecs::entity tmpl = w.lookup("bullet");
            if (!tmpl.is_valid()) {
                // 查不到是配置错误，报一次即可——storm 每帧进来，
                // 不拦会刷屏。置回 false 等人修好清单再开
                godot::UtilityFunctions::push_error(
                    "test_spawn_bullets: 模板 'bullet' 未装载，"
                    "检查 template_list.tres");
                w.get_mut<TestInput>().storm = false;
                return;
            }

            for (int k = 0; k < 30; ++k) {
                w.entity()
                    .set<SpawnRequest>({
                        .identity = tmpl,
                        .position = Position{
                            rnd(-5.0F, 5.0F),
                            rnd(2.0F, 6.0F),
                            rnd(-5.0F, 5.0F)},
                        .rotation = {},
                        .scale = {},
                    });
            }
        });

    // 寿命系统：销毁的唯一权威入口——经 bridge::despawn，
    // 先处置视图、后销毁实体，顺序即协议。
    p_w.system<Life>("expire")
        .kind(flecs::OnUpdate)
        .each([](flecs::iter &p_it, size_t p_i, Life &p_life) {
            p_life.t -= p_it.delta_time();
            if (p_life.t <= 0.0F) {
                bridge::despawn(p_it.entity(p_i));
            }
        });

}

} // namespace logic
