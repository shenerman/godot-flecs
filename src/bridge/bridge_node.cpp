/**************************************************************************/
/*  bridge/bridge_node.cpp                                                */
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


#include "bridge_node.hpp"

#include "flecs_world.hpp"
#include "logic/components.hpp"
#include "node_ref.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

namespace bridge {

void BridgeNode::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    set_rotation_order(EulerOrder::EULER_ORDER_YXZ);

    try_bind();
    
}
void BridgeNode::_enter_tree() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;   // 编辑器里摆放节点也会触发 _enter_tree，必须挡住
    }
    // 不复活条款：曾绑定过的节点重入树，只警告、不重绑。
    // 视图连接没有复活语义——复活意味着在逻辑侧和场景树之间做数据仲裁，
    // 这个仲裁没有正确答案，所以宁可脱钩且失败可见。
    if (_was_bound) {
        UtilityFunctions::push_warning(
            "BridgeNode: re-entered tree after binding; node is now detached.");
        return;
    }
    // 首次进树什么都不做：绑定留给 _ready（此时旋转序等还没就绪）
}


void BridgeNode::try_bind() {
    // 不复活条款：曾绑定过的节点再入树，直接放弃（reparent 会走到这里，
    // 节点从此脱钩——失败必须可见，不做静默重试）
    if (_was_bound) {
        UtilityFunctions::push_warning(
            "BridgeNode: re-entered tree after binding; node is now detached.");
        return;
    }

    flecs::world* w = nullptr;
    for (Node* p = get_parent(); p != nullptr; p = p->get_parent()) {
        if (auto* host = Object::cast_to<FlecsWorld>(p)) {
            w = &host->flecs_world();
            break;
        }
    }
    
    if (w == nullptr) {
        UtilityFunctions::push_error(
            "BridgeNode: no FlecsWorld ancestor. "
            "BridgeNode must be placed under a FlecsWorld node.");
        return;
    }

    _entity = w->entity();

    Vector3 rot = get_rotation();
    _entity.set<logic::Position>({ get_position().x, get_position().y, get_position().z });
    _entity.set<logic::Rotation>({ rot.x, rot.y, rot.z });
    _entity.set<logic::Scale>({ get_scale().x, get_scale().y, get_scale().z });

    _entity.set<NodeRef>({ this });

    _was_bound = true;

    UtilityFunctions::print(vformat("[bind] entity %d created for %s",
    (int64_t)_entity.id(), this->get_name()));
}

void BridgeNode::_exit_tree() {
    // 终止配对。防御性检查：实体可能已被逻辑侧销毁（M3 预留），或从未绑定成功。
    // 注意 get 返回指针：用 ->，不是 .
    if (_entity.is_valid() && _entity.has<NodeRef>()
        && _entity.get<NodeRef>().node == this) {
        _entity.remove<NodeRef>();
        UtilityFunctions::print("[unbind] NodeRef removed");
    // 实体本身不销毁——它是逻辑侧的对象，生死由逻辑决定（单一权威）
    }
}

}

