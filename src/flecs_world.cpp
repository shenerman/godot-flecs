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
#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "node_ref.h"
#include "position.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

FlecsWorld::~FlecsWorld() {
  // 场景释放时先销毁 world：flecs 析构后实体句柄全部失效，
  // 若 Godot 在此之后仍触发配对节点的 tree_exiting 回调就会访问死 world。
  // world 先走，_on_paired_node_exiting 里的 !_world 早退兜底。
  _world.reset();
}

void FlecsWorld::set_test_move_speed_x(float p_speed) {
    _test_move_speed_x = p_speed;
}
void FlecsWorld::set_test_move_speed_y(float p_speed) {
    _test_move_speed_y = p_speed;
}
float FlecsWorld::get_test_move_speed_x() const {
    return _test_move_speed_x;
}
float FlecsWorld::get_test_move_speed_y() const {
    return _test_move_speed_y;
}

void FlecsWorld::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_paired_node_exiting", "entity_id"),
                         &FlecsWorld::_on_paired_node_exiting);

    // 先绑定 getter/setter 方法，属性系统按名查找它们
    ClassDB::bind_method(D_METHOD("set_test_move_speed_x", "speed"),
                         &FlecsWorld::set_test_move_speed_x);
    ClassDB::bind_method(D_METHOD("get_test_move_speed_x"),
                         &FlecsWorld::get_test_move_speed_x);
    ClassDB::bind_method(D_METHOD("set_test_move_speed_y", "speed"),
                         &FlecsWorld::set_test_move_speed_y);
    ClassDB::bind_method(D_METHOD("get_test_move_speed_y"),
                         &FlecsWorld::get_test_move_speed_y);

    // 属性名不带下划线前缀（公开标识符）
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "test_move_speed_x",
                              PROPERTY_HINT_RANGE, "0,500,0.1"),
                 "set_test_move_speed_x", "get_test_move_speed_x");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "test_move_speed_y",
                              PROPERTY_HINT_RANGE, "0,500,0.1"),
                 "set_test_move_speed_y", "get_test_move_speed_y");
}

void FlecsWorld::_ready() {

  if (Engine::get_singleton()->is_editor_hint()) {
    return; // 编辑器里不创建 world
  }
  _world.emplace();

  register_position(*_world);
  register_node_ref(*_world);

  _world->system<const Position, const NodeRef>("SyncPositions")
      .kind(flecs::PostUpdate)
      .each([](flecs::entity /*p_e*/, const Position &p_pos,
               const NodeRef &p_nr) {
        if (p_nr.instance_id == 0) {
          return;
        }
        // [类型边界说明] Godot 脚本互操作层的整数是 int64_t（GDScript int
        // 是有符号的），instance_from_id 参数即 int64_t；引擎源码内部第一行
        // 就是 ObjectID((uint64_t)p_id) 镜像转换回无符号。此处显式 cast
        // 与引擎自身的边界转换完全一致，非权宜之计。
        Object *obj = UtilityFunctions::instance_from_id(
            static_cast<int64_t>(p_nr.instance_id));
        auto *node = Object::cast_to<Node2D>(obj);
        // ID 失效（节点已释放且清理钩子未及触发）：跳过，不崩溃
        if (node == nullptr) {
          return;
        }
        node->set_position(Vector2(p_pos.x, p_pos.y));
      });

  // ── 抛弃型测试系统（验收标准 3）：模拟任意逻辑系统手动改 Position ──
  // 合入后保留用于验收演示，接入 Pong 真实逻辑时删除。
  _world->system<Position>("TestMove")
      .kind(flecs::OnUpdate)
      .each([this](flecs::iter &p_it, size_t /*p_i*/, Position &p_pos) {
        p_pos.x += _test_move_speed_x * p_it.delta_time();
        p_pos.y += _test_move_speed_y * p_it.delta_time();
      });

  // 一次性心跳系统：证明 pipeline 确实在跑。
  // 无组件的 system 在 flecs 中匹配不到任何实体、不会执行，
  // 所以挂一个 throwaway tag 作为匹配目标；interval(1.0) 限频为每秒一次。
    struct Heartbeat {}; // tag：无数据，纯标记

    _world->entity("heartbeat").add<Heartbeat>();
    _world->system<Heartbeat>("PrintHeartbeat")
            .interval(1.0)
            .each([this](flecs::entity, Heartbeat) {
                UtilityFunctions::print("[flecs] heartbeat #", ++_heartbeat_ticks);
            });

    _spawn_paired_entities();
}

void FlecsWorld::_spawn_paired_entities() {
    Node *container = get_node_or_null("Entities");
    if (container == nullptr) {
        UtilityFunctions::print("FlecsWorld: no 'Entities' container, skip spawning");
        return;
    }

    int count = 0;
    const Array children = container->get_children();
    for (const Variant &child_variant : children) {
        auto *node = Object::cast_to<Node2D>(child_variant);
        if (node == nullptr) {
            continue;
        }

        const Vector2 pos = node->get_position();
        flecs::entity e = _world->entity()
                                  .set<Position>({ pos.x, pos.y })
                                  .set<NodeRef>({ node->get_instance_id() });

        node->connect("tree_exiting",
                      Callable(this, "_on_paired_node_exiting").bind(e.id()));
        ++count;
    }
    UtilityFunctions::print("FlecsWorld: spawned ", count, " paired entities");
}


void FlecsWorld::_on_paired_node_exiting(int64_t p_entity_id) {
    if (!_world) {
        return; // world 已析构（节点析构晚于 FlecsWorld 时兜底）
    }
    _world->entity(static_cast<flecs::id_t>(p_entity_id)).destruct();
}

void FlecsWorld::_physics_process(double p_delta) {
  if (Engine::get_singleton()->is_editor_hint() || !_world) {
    return;
  }

  _world->progress(static_cast<float>(p_delta));
}

} // namespace godot
