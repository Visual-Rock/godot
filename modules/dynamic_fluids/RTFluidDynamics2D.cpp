#include "RTFluidDynamics2D.h"

RTFluidDynamics2D::RTFluidDynamics2D(/* args */)
{
    print_line("Constructor");
}

RTFluidDynamics2D::~RTFluidDynamics2D()
{
}

void RTFluidDynamics2D::add_velocity(Vector2 position, float amount)
{

}

void RTFluidDynamics2D::add_density(Vector2 position, float amount)
{

}

void RTFluidDynamics2D::set_grid_size(Vector2 size)
{
    grid_size = size;
}

Vector2 RTFluidDynamics2D::get_grid_size()
{
    return grid_size;
}

void RTFluidDynamics2D::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("add_velocity", "position", "amount"), &RTFluidDynamics2D::add_velocity);
    ClassDB::bind_method(D_METHOD("add_density", "position", "amount"), &RTFluidDynamics2D::add_density);
    ClassDB::bind_method(D_METHOD("set_grid_size", "size"), &RTFluidDynamics2D::set_grid_size);
    ClassDB::bind_method(D_METHOD("get_grid_size"), &RTFluidDynamics2D::get_grid_size);

    ADD_GROUP("Grid", "");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "grid_size"), "set_grid_size", "get_grid_size");
}