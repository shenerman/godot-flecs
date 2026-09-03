#include "example_class.h"

void ExampleClass::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &ExampleClass::print_type);
}

void ExampleClass::print_type(const Variant &p_variant) const {
	UtilityFunctions::print(
		"ExampleClass (", get_class(), "): Variant type: ",
		Variant::get_type_name(p_variant.get_type()));
}
