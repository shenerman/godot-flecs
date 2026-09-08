/**************************************************************************/
/*  bridge/flecs_world.h                                                  */
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

/* bridge/flecs_world.hpp */
#pragma once

#include <flecs.h>

#include <optional>

#include <godot_cpp/classes/node.hpp>

#include "bridge/template_list.hpp"

namespace bridge {

class FlecsWorld : public godot::Node {
    GDCLASS(FlecsWorld, godot::Node) // NOLINT

public:
    FlecsWorld() = default;
    ~FlecsWorld() override;

    void _enter_tree() override;
    void _physics_process(double p_delta) override;
    void _notification(int p_what);

    [[nodiscard]] godot::Ref<TemplateList> get_template_list() const;
    void set_template_list(const godot::Ref<TemplateList> &p_list);

    [[nodiscard]] bool has_world() const { return _world.has_value(); }
    [[nodiscard]] flecs::world &flecs_world() { return *_world; }

protected:
    static void _bind_methods();

private:
    std::optional<flecs::world> _world;
    godot::Ref<TemplateList> _template_list;
};

} // namespace bridge
