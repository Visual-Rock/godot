#include "register_types.h"

#include "core/object/class_db.h"

#include "RTFluidDynamics2D.h"

void register_dynamic_fluids_types() {
	ClassDB::register_class<RTFluidDynamics2D>();
}

void unregister_dynamic_fluids_types() {
	
}
