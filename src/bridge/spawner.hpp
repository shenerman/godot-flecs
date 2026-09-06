#pragma once

#include <flecs.h>

namespace godot {
class Node;
}

namespace bridge {

class TemplateRegistry;

void register_spawner(flecs::world &p_w,
                      TemplateRegistry *p_registry,
                      godot::Node *p_view_host);

}  // namespace bridge
