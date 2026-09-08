/**************************************************************************/
/*  bridge/flecs_world.cpp                                                */
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

/* bridge/flecs_world.cpp */
#include "bridge/flecs_world.hpp"

#include "bridge/despawn.hpp"
#include "bridge/node_ref.hpp"
#include "bridge/spawner.hpp"
#include "bridge/sync_transform.hpp"
#include "bridge/template_list.hpp"
#include "bridge/template_loader.hpp"
#include "logic/components.hpp"

#include <godot_cpp/classes/engine.hpp>

namespace bridge {

FlecsWorld::~FlecsWorld() {
	_world.reset();
}

void FlecsWorld::_enter_tree() {
	if (godot::Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	if (_world) {
		return;
	}

	_world.emplace();

	_world->component<ViewOf>()
			.add(flecs::OnDeleteTarget, flecs::Delete);

	// 装载流水线：清单 → 模板实体（查重/验场景/验牌全在启动期报错）
	load_templates(*_world, _template_list);

	register_spawner(*_world, this);

	logic::register_components(*_world);
	register_sync_transform(*_world);
	logic::register_systems(*_world);
	register_despawn(*_world);

	_world->set<logic::TestInput>({});
	_world->get_mut<logic::TestInput>().storm = true;
}

void FlecsWorld::_physics_process(double p_delta) {
	if (godot::Engine::get_singleton()->is_editor_hint() || !_world) {
		return;
	}
	_world->progress(static_cast<float>(p_delta));
}

void FlecsWorld::_notification(int p_what) {
	if (p_what == NOTIFICATION_EXIT_TREE) {
		if (!_world.has_value()) {
			return;
		}
		// queue free
		_world->each<NodeRef>(
				[](flecs::entity, NodeRef &p_ref) {
					if (p_ref.node != nullptr) {
						p_ref.node->call("free"); // 退出期同步清理
						p_ref.node = nullptr;
					}
				});
	}
}

void FlecsWorld::set_template_list(const godot::Ref<TemplateList> &p_list) {
	_template_list = p_list;
}

godot::Ref<TemplateList> FlecsWorld::get_template_list() const {
	return _template_list;
}

void FlecsWorld::_bind_methods() {
	godot::ClassDB::bind_method(
			godot::D_METHOD("set_template_list", "list"),
			&FlecsWorld::set_template_list);
	godot::ClassDB::bind_method(
			godot::D_METHOD("get_template_list"),
			&FlecsWorld::get_template_list);

	ADD_PROPERTY(
			godot::PropertyInfo(godot::Variant::OBJECT, "template_list",
					godot::PropertyHint::PROPERTY_HINT_RESOURCE_TYPE,
					"TemplateList"),
			"set_template_list", "get_template_list");
}

} // namespace bridge
