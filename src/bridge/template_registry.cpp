/**************************************************************************/
/*  bridge/template_registry.cpp                                          */
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

#include "bridge/template_registry.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace bridge {

void TemplateRegistry::set_templates(const godot::Array &p_v) {
    _templates = p_v;
}

godot::Array TemplateRegistry::get_templates() const {
    return _templates;
}

godot::PackedScene *TemplateRegistry::find(logic::TemplateId p_id) const {
    for (int i = 0; i < _templates.size(); ++i) {
        const godot::Dictionary &entry = _templates[i];

        // Variant 里 identity 以 int64 存放（编辑器侧），先去符号再
        // 对齐到无符号 TemplateId——两次 cast 各有明确含义，不合并
        if (static_cast<uint64_t>(static_cast<int64_t>(entry["identity"])) == p_id) {
            // 场景字段若被填错类型，cast_to 返回 nullptr 原样上抛，
            // 由调用方（spawner）在写点报错——本类不静默也不越权
            return godot::Object::cast_to<godot::PackedScene>(entry["scene"]);
        }
    }
    return nullptr;
}

void TemplateRegistry::_bind_methods() {
    godot::ClassDB::bind_method(
        godot::D_METHOD("set_templates", "templates"),
        &TemplateRegistry::set_templates);
    godot::ClassDB::bind_method(
        godot::D_METHOD("get_templates"),
        &TemplateRegistry::get_templates);

    ADD_PROPERTY(
        godot::PropertyInfo(godot::Variant::ARRAY, "templates"),
        "set_templates", "get_templates");
}

}  // namespace bridge
