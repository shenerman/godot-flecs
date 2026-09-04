/**************************************************************************/
/*  flecs_world.cpp                                                         */
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

#include "flecs_world.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


namespace godot {

void FlecsWorld::_bind_methods() {
    // Issue 2 故意不暴露任何配置；Issue 3 接 Pong 实体时再补 API
}

void FlecsWorld::_ready() {
    
    if (Engine::get_singleton()->is_editor_hint()) {
        return; // 编辑器里不创建 world
    }
    _world.emplace();

    // 一次性心跳系统：证明 pipeline 确实在跑。
    // 无组件的 system 在 flecs 中匹配不到任何实体、不会执行，
    // 所以挂一个 throwaway tag 作为匹配目标；interval(1.0) 限频为每秒一次。
    struct Heartbeat {}; // tag：无数据，纯标记

    _world->entity("heartbeat").add<Heartbeat>();
    _world->system<Heartbeat>("PrintHeartbeat")
            .interval(1.0)
            .each([](flecs::entity, Heartbeat) {
                static int ticks = 0;
                UtilityFunctions::print("[flecs] heartbeat #", ++ticks);
            });
}

void FlecsWorld::_physics_process(double p_delta) {
    if (Engine::get_singleton()->is_editor_hint() || !_world) {
        return;
    }

    _world->progress(static_cast<float>(p_delta));
}

} // namespace godot
