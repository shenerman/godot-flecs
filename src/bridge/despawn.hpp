#pragma once

#include <flecs.h>

#include "bridge/node_ref.hpp"

namespace bridge {

inline void despawn(flecs::entity p_e) {
    if (p_e.has<NodeRef>()) {
        const auto &ref = p_e.get<NodeRef>();
        if (ref.node != nullptr) {
            ref.node->queue_free();
        }
    }
    p_e.destruct();
}


}  // namespace bridge
