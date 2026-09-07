/**************************************************************************/
/*  bridge/spawner.cpp                                                    */
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

/* bridge/spawner.cpp */
#include "bridge/spawner.hpp"

#include <flecs.h>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "bridge/bridge_node.hpp"
#include "bridge/despawn.hpp"
#include "bridge/node_ref.hpp"
#include "bridge/template_list.hpp"
#include "logic/components.hpp"
#include "logic/templates.hpp"

namespace bridge {


void register_spawner(flecs::world &p_w, godot::Node *p_view_host) {
    p_w.system<const logic::SpawnRequest>("spawn_from_requests")
        // PreUpdate 消费上一帧 OnUpdate 产出的请求——产与消相位错开，
        // 排序由管线硬保证，不依赖同相位内的声明顺序
        .kind(flecs::PreUpdate)
        .each([p_view_host](flecs::iter &p_it, size_t p_i,
                            const logic::SpawnRequest &p_req) {
            flecs::entity e = p_it.entity(p_i);

            // 1. 模板句柄直取视图与逻辑种类——find() 间接层消失。
            //    句柄无效 = 未装载或被清，报错责任在写点（此处），
            //    带上下文并干净处置请求
            const auto *vt =
                p_req.identity.try_get<bridge::ViewTemplate>();
            if (vt == nullptr) {
                godot::UtilityFunctions::push_error(godot::vformat(
                    "spawn: 无效模板句柄（'%s' 未装载或已失效）",
                    godot::String(p_req.identity.name())));
                despawn(e);
                return;
            }

            // 2. 逻辑模板直调：装配已由装载期存上 prefab
            const auto *lt = p_req.identity.try_get<logic::LogicTemplate>();
            if (lt == nullptr || !lt->build(e)) {
                godot::UtilityFunctions::push_error(godot::vformat(
                    "spawn: 模板 '%s' 无可用逻辑模板",
                    godot::String(p_req.identity.name())));
                despawn(e);
                return;
            }

            // 3. 实例化视图，根必须是 BridgeNode（场景模板契约 1；
            //    装载期已预检一次，此处兜底防御运行期场景被换）
            auto *view =
                godot::Object::cast_to<BridgeNode>(vt->scene->instantiate());
            if (view == nullptr) {
                godot::UtilityFunctions::push_error(godot::vformat(
                    "spawn: 模板 '%s' 的场景根节点缺少 BridgeNode",
                    godot::String(p_req.identity.name())));
                despawn(e);
                return;
            }

            // 4. 产品的位姿组件：来自请求，optional 缺席 = 产品无此组件，
            //    sync_transform 会跳过缺席项——"场景出厂值"契约不变
            if (p_req.position) {
                e.set<logic::Position>(
                    {p_req.position->x, p_req.position->y, p_req.position->z});
            }
            if (p_req.rotation) {
                e.set<logic::Rotation>(
                    {p_req.rotation->x, p_req.rotation->y, p_req.rotation->z});
            }
            if (p_req.scale) {
                e.set<logic::Scale>(
                    {p_req.scale->x, p_req.scale->y, p_req.scale->z});
            }

            // 5. 视图实体：一行记录 = 一个视图，只扛 NodeRef，不碰逻辑。
            //    边挂在视图上、指向产品——连接的唯一真源（1:N 时重复本段即可）
            flecs::entity view_entity = p_it.world().entity();
            view_entity.set<NodeRef>({view});
            view_entity.add<ViewOf>(e);

            // 6. 出生帧位姿：sync 在 PostUpdate 才跑，出生帧手动对齐一次
            //    （覆盖位姿仍按 optional 缺席不动）
            if (p_req.position) {
                view->set_position(
                    {p_req.position->x, p_req.position->y, p_req.position->z});
            }
            if (p_req.rotation) {
                view->set_rotation(
                    {p_req.rotation->x, p_req.rotation->y, p_req.rotation->z});
            }
            if (p_req.scale) {
                view->set_scale(
                    {p_req.scale->x, p_req.scale->y, p_req.scale->z});
            }

            // 7. 入树。BridgeNode 不再认识任何实体——验牌在 add_child 的
            //    _enter_tree 里照旧发生，实体身份由 pair 边承载
            p_view_host->add_child(view);
            e.remove<logic::SpawnRequest>();
        });
}

} // namespace bridge

