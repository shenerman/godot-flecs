/**************************************************************************/
/*  bridge/despawn.cpp                                                    */
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

/* bridge/despawn.cpp —— MIT 头省略，补回 */

#include "bridge/despawn.hpp"

#include "bridge/node_ref.hpp"
#include "logic/components.hpp"

#include <godot_cpp/classes/node.hpp>

#include <flecs.h>

namespace bridge {

void register_despawn(flecs::world &p_w) {
	// 视图处置：NodeRef 被移除时（级联删触发）释放 Godot 节点
	p_w.observer<NodeRef>()
			.event(flecs::OnRemove)
			.each([](flecs::iter & /*p_it*/, size_t /*p_i*/,
						  NodeRef &p_ref) {
				if (p_ref.node != nullptr) {
					p_ref.node->queue_free();
					p_ref.node = nullptr; // 防御：杜绝二次 queue_free
				}
			});

	// ---- Despawn 标记的消费端 ----
	// Despawn 是空类型（tag）：flecs 禁止 tag 以引用传入 each
	// （官方破坏性变更，防 UB），必须按值——见签名表
	p_w.system<logic::Despawn>("process_despawn")
			.kind(flecs::PostUpdate)
			.each([](flecs::entity p_e, logic::Despawn /*p_tag*/) {
				despawn(p_e);
			});
}

void despawn(flecs::entity p_product) {
	p_product.destruct();
}

} // namespace bridge
