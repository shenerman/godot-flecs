/**************************************************************************/
/*  flecs_world.h                                                         */
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
#include <optional>

namespace godot {

// 逻辑世界的宿主。职责仅三件：
// 创建/销毁 world、注册同步系统、每物理帧推进。
// 配对（出生/绑定/解绑）不在这里——由 BridgeNode 自注册。
class FlecsWorld : public Node {
    GDCLASS(FlecsWorld, Node) // NOLINT

public:
    FlecsWorld() = default;
    ~FlecsWorld() override;

    void _enter_tree() override;
    void _physics_process(double p_delta) override;

    [[nodiscard]] flecs::world& flecs_world() { return *_world; }

protected:
    static void _bind_methods() {}

private:
    // optional 而非直接成员：Node 构造发生在编辑器实例化时，
    std::optional<flecs::world> _world;
};

}
