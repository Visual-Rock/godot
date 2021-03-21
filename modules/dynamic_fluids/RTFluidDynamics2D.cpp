#include "RTFluidDynamics2D.h"

RTFluidDynamics2D::RTFluidDynamics2D(/* args */)
{
}

RTFluidDynamics2D::~RTFluidDynamics2D()
{
}

void RTFluidDynamics2D::test()
{
    print_line("Test");
}

void RTFluidDynamics2D::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("test"), &RTFluidDynamics2D::test);
}