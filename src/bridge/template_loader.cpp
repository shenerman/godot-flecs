/**************************************************************************/
/*  bridge/template_loader.cpp                                            */
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

#include "bridge/template_loader.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "bridge/bridge_node.hpp"
#include "bridge/template_list.hpp"
#include "logic/templates.hpp"

namespace bridge {

namespace {

// 验牌：实例化一次确认根节点是桥接系，随后立刻释放。
// 错配在按下运行键的瞬间暴露，不等第一次 spawn
bool verify_scene_root(
        const godot::Ref<godot::PackedScene> &p_scene,
        const godot::String &p_name) {
    if (p_scene.is_null()) {
        godot::UtilityFunctions::push_error(
            "load_templates: 模板 '", p_name, "' 未配置场景，已跳过");
        return false;
    }

    godot::Node *root = p_scene->instantiate();
    if (root == nullptr) {
        godot::UtilityFunctions::push_error(
            "load_templates: 模板 '", p_name,
            "' 的场景实例化失败，已跳过");
        return false;
    }

    const bool bridged = godot::Object::cast_to<BridgeNode>(root) != nullptr;

    // 释放纪律：instantiate 的产物必须走 call("free")，不得直调 free()。
    // 这是本项目定下的释放契约——释放动作无条件执行、
    // 且放在所有结论判断之前，杜绝后续改动插 early return 导致泄漏
    root->call("free");

    if (!bridged) {
        godot::UtilityFunctions::push_error(
            "load_templates: 模板 '", p_name,
            "' 的场景根节点不是 BridgeNode，已跳过");
        return false;
    }
    return true;
}

} // namespace

void load_templates(flecs::world &p_w, const godot::Ref<TemplateList> &p_list) {
    if (p_list.is_null()) {
        godot::UtilityFunctions::push_error(
            "load_templates: 未配置模板清单，spawn 通道将不可用");
        return;
    }

    const godot::TypedArray<TemplateEntry> entries = p_list->get_entries();
    for (int i = 0; i < entries.size(); ++i) {
        const godot::Ref<TemplateEntry> entry = entries[i];
        if (entry.is_null()) {
            godot::UtilityFunctions::push_error(
                "load_templates: entries[", i, "] 不是 TemplateEntry，已跳过");
            continue;
        }

        const godot::String name = entry->get_template_name();
        if (name.is_empty()) {
            godot::UtilityFunctions::push_error(
                "load_templates: entries[", i, "] 未填模板名，已跳过");
            continue;
        }

        // a. 查重：flecs 同名创建是 get_or_create 语义，不拦会静默合并
        const godot::CharString utf8 = name.utf8();
        if (p_w.lookup(utf8.get_data()).is_valid()) {
            godot::UtilityFunctions::push_error(
                "load_templates: 模板名 '", name, "' 重复，已跳过");
            continue;
        }

        // b + e. 验场景、验牌
        if (!verify_scene_root(entry->get_scene(), name)) {
            continue;
        }

        // d. 解析逻辑模板：nullptr = 设计师填了 logic 侧没有的名字
        const logic::BuildFn build =
            logic::build_from_name(std::string_view(utf8.get_data()));
        if (build == nullptr) {
            godot::UtilityFunctions::push_error(
                "load_templates: 模板名 '", name,
                "' 没有对应的逻辑模板，已跳过");
            continue;
        }

        // c. 建 prefab 实体：装配存上 prefab，spawn 侧零查表
        p_w.prefab(utf8.get_data())
            .set<ViewTemplate>({entry->get_scene()})
            .set<logic::LogicTemplate>({build});

    }
}

} // namespace bridge
