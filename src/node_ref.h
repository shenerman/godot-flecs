/**************************************************************************/
/*  node_ref.h                                                         */
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
// NodeRef：实体 ↔ 场景节点的映射，也是纯数据（uint64_t，Godot 的实例 ID）。
// 设计说明（对应 issue 的 lifetime caution）：
//   - 不存 Node2D*：裸指针在场景重载后悬空且不可检测；
//   - 存实例 ID：每帧通过 instance_from_id 解引用，失效返回 null，同步系统跳过；
//   - 配对节点的 tree_exiting 信号会触发 entity.destruct()（见 flecs_world.cpp），
//     双向清理，防止 flecs 实体跨场景重载持有死 ID。
#include "flecs.h"
#include <cstdint>
struct NodeRef {
    uint64_t instance_id { 0 };
};

inline void register_node_ref(flecs::world &p_world) {
    p_world.component<NodeRef>()
            .member<uint64_t>("instance_id");
}