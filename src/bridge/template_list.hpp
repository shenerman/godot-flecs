/* bridge/template_list.hpp */
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
