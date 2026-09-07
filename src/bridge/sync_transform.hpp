/**************************************************************************/
/*  bridge/sync_transform.hpp                                             */
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

#include "logic/components.hpp"
#include "bridge/node_ref.hpp"

namespace bridge {

// 查询目标从产品换成视图实体：NodeRef 挂在视图上，产品由 (ViewOf) 边指向。
// 1:N 免费获得——同一产品被多个视图实体各持一条边，逐个刷位姿
inline void register_sync_transform(flecs::world& p_world) {
    p_world.system<const NodeRef>("SyncTransform")
        .kind(flecs::PostUpdate)
        .each([](flecs::entity p_view, const NodeRef &p_ref) {
            if (p_ref.node == nullptr) {
                godot::UtilityFunctions::push_warning("View entity has no node assigned to it");
                return;
            }

            // 连接的真源是边：视图 ──(ViewOf)──> 产品。一次表内查找，O(1)
            const flecs::entity product = p_view.target<ViewOf>();
            if (!product.is_alive()) {
                // 防御：绕过 despawn() 直接 destruct() 产品的外来路径。
                // 正常路径下 ViewOf 的 (OnDeleteTarget, Delete) 策略会在
                // 产品死亡时连带删除视图，走不到这里
                return;
            }

            // v4：可选读取用 try_get（返回 const T*，缺席为 nullptr）。
            // 千万别用 get<T>()——它返回 const T&，缺组件时是断言/未定义行为
            const auto *pos = product.try_get<logic::Position>();
            const auto *rot = product.try_get<logic::Rotation>();
            const auto *scl = product.try_get<logic::Scale>();
            if (pos == nullptr && rot == nullptr && scl == nullptr) {
                return; // 产品尚未接位姿组件，等逻辑系统填
            }

            // NodeRef.node 已是 Node3D*，直写无需 cast
            if (pos != nullptr) { p_ref.node->set_position({pos->x, pos->y, pos->z}); }
            if (rot != nullptr) { p_ref.node->set_rotation({rot->x, rot->y, rot->z}); }
            if (scl != nullptr) { p_ref.node->set_scale({scl->x, scl->y, scl->z}); }
        });
}

} // namespace bridge