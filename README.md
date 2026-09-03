# godot-flecs

A Godot 4 project that combines the [flecs](https://github.com/SanderMertens/flecs)
ECS with Godot via GDExtension.

Flecs runs the game logic (entities, components, systems), while Godot handles
rendering, input, and scene management. A thin bridge keeps the two worlds in sync.

## Structure

- `src/` — bridge code and game systems (C++)
- `thirdparty/` — third-party libraries (flecs, etc.)
- `godot-cpp/` — GDExtension bindings (submodule)
- `project/` — Godot project

## Status

Early development — no playable build yet.
