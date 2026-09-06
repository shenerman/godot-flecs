/**************************************************************************/
/*  flecs_world.cpp                                                       */
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

#include "bridge/flecs_world.hpp"

#include <godot_cpp/classes/engine.hpp>

#include "bridge/spawner.hpp"
#include "bridge/sync_transform.hpp"
#include "bridge/template_registry.hpp"
#include "logic/components.hpp"

namespace godot {

FlecsWorld::~FlecsWorld() {
    _world.reset();
}

void FlecsWorld::_enter_tree() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    if (_world) {
        return;
    }

    // Registry 解析在系统注册之前——写点报错，不静默
    auto *registry = godot::Object::cast_to<bridge::TemplateRegistry>(
        get_node_or_null(_template_registry_path));
    if (registry == nullptr) {
        godot::UtilityFunctions::push_error(
            "FlecsWorld: template_registry_path 未配置或目标不是 TemplateRegistry，"
            "spawn 通道将不可用");
    }

    _world.emplace();
    logic::register_components(*_world);
    bridge::register_sync_transform(*_world);
    logic::register_systems(*_world);
    bridge::register_spawner(*_world, registry, this);

    _world->set<logic::TestInput>({});
    _world->get_mut<logic::TestInput>().storm = true;  
}

void FlecsWorld::_physics_process(double p_delta) {
    if (Engine::get_singleton()->is_editor_hint() || !_world) {
        return;
    }
    _world->progress(static_cast<float>(p_delta));
}

NodePath FlecsWorld::get_template_registry_path() const {
    return _template_registry_path;
}

void FlecsWorld::set_template_registry_path(const NodePath &p_path) {
    _template_registry_path = p_path;
}

void FlecsWorld::_bind_methods() {
    godot::ClassDB::bind_method(
        godot::D_METHOD("set_template_registry_path", "path"),
        &FlecsWorld::set_template_registry_path);
    godot::ClassDB::bind_method(
        godot::D_METHOD("get_template_registry_path"),
        &FlecsWorld::get_template_registry_path);

    ADD_PROPERTY(
        godot::PropertyInfo(godot::Variant::NODE_PATH, "template_registry_path"),
        "set_template_registry_path", "get_template_registry_path");
}

}  // namespace godot

