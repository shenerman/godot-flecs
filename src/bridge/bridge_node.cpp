/**************************************************************************/
/*  bridge/bridge_node.cpp                                                */
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


#include "bridge/bridge_node.hpp"

#include <godot_cpp/classes/engine.hpp>

namespace bridge {

bool BridgeNode::is_bound() const {
    return _entity_id != 0;
}

void BridgeNode::assign_entity(uint64_t p_entity_id) {
    if (is_bound()) {
        ERR_PRINT("BridgeNode::assign_entity: already bound, 2a ordering violated");
        return;
    }
    _entity_id = p_entity_id;
}

void BridgeNode::_enter_tree() {
    if (godot::Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    if (!is_bound()) {
        ERR_PRINT("BridgeNode::_enter_tree: unbound BridgeNode entered the tree — "
                  "assign_entity must be called before add_child");
    }
}

void BridgeNode::_exit_tree() {
    _entity_id = 0;
}

void BridgeNode::_bind_methods() {
}

}  // namespace bridge