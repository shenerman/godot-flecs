#pragma once
#include <flecs.h>

#include "logic/components.hpp"
#include "bridge/node_ref.hpp"

namespace bridge {

inline void register_sync_transform(flecs::world& p_world) {
    p_world.system<const logic::Position, const logic::Rotation,
             const logic::Scale, const NodeRef>("SyncTransform")
        .kind(flecs::PostUpdate)
        .each([](flecs::entity, const logic::Position& p_pos, const logic::Rotation& p_rot,
                 const logic::Scale& p_scale, const NodeRef& p_ref) {
            if (p_ref.node == nullptr) {
                godot::UtilityFunctions::push_warning(
                "Entity has no node assigned to it");
                return; 
            }
            p_ref.node->set_position({p_pos.x, p_pos.y, p_pos.z});
            p_ref.node->set_rotation({p_rot.x, p_rot.y, p_rot.z});
            p_ref.node->set_scale({p_scale.x, p_scale.y, p_scale.z});
        });
}

}
