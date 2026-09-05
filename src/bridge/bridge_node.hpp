/**************************************************************************/
/*  bridge/bridge_node.h                                                  */
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
#include <godot_cpp/classes/node3d.hpp>

namespace bridge {

// 场景侧的配对端点：编辑器里摆在实体出生位置的标记节点。
// 职责：锁死旋转语义 → 出生导入（G3）→ 建立配对 → 离树终止配对。
// 配对数据本身在实体的 NodeRef 组件上，本类只管生命周期。
// 实体生死归逻辑侧：_exit_tree 只摘 NodeRef，不销毁实体。
class BridgeNode : public godot::Node3D {
    GDCLASS(BridgeNode, godot::Node3D) // NOLINT

public:
    void _ready() override;
    void _enter_tree() override;
    void _exit_tree() override;

protected:
    static void _bind_methods() {}

private:
    void try_bind();

    flecs::entity _entity{};
    bool _was_bound = false;
};

}

