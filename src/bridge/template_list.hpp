/**************************************************************************/
/*  bridge/template_list.hpp                                              */
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

#pragma once

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource.hpp>

namespace bridge {

struct ViewTemplate {
	godot::Ref<godot::PackedScene> scene;
};

// 清单条目：设计师在 Inspector 里逐条编辑
class TemplateEntry : public godot::Resource {
	GDCLASS(TemplateEntry, godot::Resource) // NOLINT

public:
	// 注意不用 set_name/get_name——Resource 自身有同名方法（resource_name）
	void set_template_name(const godot::String &p_name);
	[[nodiscard]] godot::String get_template_name() const;

	void set_scene(const godot::Ref<godot::PackedScene> &p_scene);
	[[nodiscard]] godot::Ref<godot::PackedScene> get_scene() const;

protected:
	static void _bind_methods();

private:
	godot::String _template_name;
	godot::Ref<godot::PackedScene> _scene;
};

// 清单本体：FlecsWorld 导出引用，启动期交给装载器
class TemplateList : public godot::Resource {
	GDCLASS(TemplateList, godot::Resource) // NOLINT

public:
	void set_entries(const godot::TypedArray<bridge::TemplateEntry> &p_entries);
	[[nodiscard]] godot::TypedArray<bridge::TemplateEntry> get_entries() const;

protected:
	static void _bind_methods();

private:
	godot::TypedArray<bridge::TemplateEntry> _entries;
};

} // namespace bridge
