#include "register_types.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "flecs_world.hpp"
#include "bridge/bridge_node.hpp"

using namespace godot;

namespace
{
	void initialize_gdextension_types(ModuleInitializationLevel p_level)
	{
		if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
			return;
		}
		GDREGISTER_CLASS(FlecsWorld)
		GDREGISTER_CLASS(bridge::BridgeNode)
	}

	void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
		if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
			return;
		}
	}
} // namespace

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT godot_flecs_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_gdextension_types);
		init_obj.register_terminator(uninitialize_gdextension_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}