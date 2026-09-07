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

#pragma once

#include <flecs.h>
#include <godot_cpp/classes/node.hpp>

namespace godot {

// 逻辑世界的宿主。职责仅四件：创建/销毁 world、注册关系清理策略、
// 解析模板注册表并注册全部系统、每物理帧推进。
// 节点的出生与回收不在本类——视图实体由 spawner 创建，
// 节点回收统一走 OnRemove<NodeRef> hook（despawn.cpp）
class FlecsWorld : public Node {
    GDCLASS(FlecsWorld, Node) // NOLINT

public:
    FlecsWorld() = default;
    ~FlecsWorld() override;

    void _enter_tree() override;
    void _physics_process(double p_delta) override;
    void _notification(int p_what);

    // 编辑器接线：指向场景里的 TemplateRegistry 节点
    [[nodiscard]] NodePath get_template_registry_path() const;
    void set_template_registry_path(const NodePath &p_path);

    [[nodiscard]] bool has_world() const { return _world.has_value(); }
    [[nodiscard]] flecs::world &flecs_world() { return *_world; }

protected:
    static void _bind_methods();

private:
    // optional 而非直接成员：Node 构造发生在编辑器实例化时，
    // world 只应在运行时创建
    std::optional<flecs::world> _world;
    NodePath _template_registry_path;
};

} // namespace godot