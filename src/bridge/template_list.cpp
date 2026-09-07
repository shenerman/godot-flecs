/* bridge/template_list.cpp */
#include "bridge/template_list.hpp"

namespace bridge {

// ---- TemplateEntry ----

void TemplateEntry::set_template_name(const godot::String &p_name) {
    _template_name = p_name;
}

godot::String TemplateEntry::get_template_name() const {
    return _template_name;
}

void TemplateEntry::set_scene(const godot::Ref<godot::PackedScene> &p_scene) {
    _scene = p_scene;
}

godot::Ref<godot::PackedScene> TemplateEntry::get_scene() const {
    return _scene;
}

void TemplateEntry::_bind_methods() {
    godot::ClassDB::bind_method(
        godot::D_METHOD("set_template_name", "name"),
        &TemplateEntry::set_template_name);
    godot::ClassDB::bind_method(
        godot::D_METHOD("get_template_name"),
        &TemplateEntry::get_template_name);
    godot::ClassDB::bind_method(
        godot::D_METHOD("set_scene", "scene"),
        &TemplateEntry::set_scene);
    godot::ClassDB::bind_method(
        godot::D_METHOD("get_scene"),
        &TemplateEntry::get_scene);

    ADD_PROPERTY(
        godot::PropertyInfo(godot::Variant::STRING, "template_name"),
        "set_template_name", "get_template_name");
    ADD_PROPERTY(
        godot::PropertyInfo(godot::Variant::OBJECT, "scene",
                            godot::PropertyHint::PROPERTY_HINT_RESOURCE_TYPE,
                            "PackedScene"),
        "set_scene", "get_scene");
}

// ---- TemplateList ----

void TemplateList::set_entries(
    const godot::TypedArray<bridge::TemplateEntry> &p_entries) {
    _entries = p_entries;
}

godot::TypedArray<bridge::TemplateEntry> TemplateList::get_entries() const {
    return _entries;
}

void TemplateList::_bind_methods() {
    godot::ClassDB::bind_method(
        godot::D_METHOD("set_entries", "entries"),
        &TemplateList::set_entries);
    godot::ClassDB::bind_method(
        godot::D_METHOD("get_entries"),
        &TemplateList::get_entries);

    ADD_PROPERTY(
        godot::PropertyInfo(godot::Variant::ARRAY, "entries"),
        "set_entries", "get_entries");
}

} // namespace bridge
