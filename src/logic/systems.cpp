/**************************************************************************/
/*  bridge/systems.cpp                                                    */
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

// ============================================================
// 逻辑系统 —— 只读写组件、声明生灭。
// 管线内系统自动处于 defer 模式：结构变更入队，
// progress() 返回前合并完毕（帧契约推论 6）。
// ============================================================
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
    static std::mt19937 rng{42U};    // NOLINT(cert-msc32-c, cert-msc51-cpp)
    return std::uniform_real_distribution<float>(p_lo, p_hi)(rng);
}

}  // namespace

void register_systems(flecs::world &p_w) {

    // 测试 T5：子弹流。请求实体只挂 SpawnRequest——
    // 标签、寿命等模板数据由 2a 按 TemplateId 配给真正的子弹实体。
    p_w.system<const TestInput>("test_spawn_bullets")
        .kind(flecs::OnUpdate)
        .run([](flecs::iter &p_it) {
            flecs::world w = p_it.world();

            if (!w.has<TestInput>() || !w.get<TestInput>().storm) {
                return;
            }
            
            for (int k = 0; k < 5; ++k) {
                w.entity()
                    .set<SpawnRequest>({
                        .identity = BULLET_ID,
                        .position = Position{
                            rnd(-5.0F, 5.0F),
                            rnd(2.0F, 6.0F),
                            rnd(-5.0F, 5.0F)},
                        .rotation = {},
                        .scale    = {},
                    });
            }
        });


    // 寿命系统：销毁的唯一权威入口——经 bridge::despawn，
    // 先处置视图、后销毁实体，顺序即协议。
    p_w.system<Life>("expire")
        .kind(flecs::OnUpdate)
        .each([](flecs::iter &p_it, size_t p_i, Life &p_life) {
            // 调试脚手架：只在帧内第一个实体上计数——static 若对所有
            // 回调累加，频率会随实体数线性放大（storm 下每秒几十次）。
            // 每帧最多一次 × 每 300 帧输出 ≈ 5 秒一条（物理帧 60Hz）
            static int s_dbg_frames = 0;
            const bool dbg_tick = (p_i == 0 && ++s_dbg_frames % 300 == 0);

            p_life.t -= p_it.delta_time();
            if (p_life.t <= 0.0F) {
                if (dbg_tick) { // 快死的这一帧恰好是打印帧：一并说明
                    godot::UtilityFunctions::print(
                        "expire: entity despawned, alive was ", p_it.count());
                }
                bridge::despawn(p_it.entity(p_i));
                return;
            }

            if (dbg_tick) {
                godot::UtilityFunctions::print(
                    "expire alive: ", p_it.count(),
                    " delta: ", p_it.delta_time(),
                    " remaining: ", p_life.t);
            }
        });
}

}  // namespace logic
