/**
 * 
 *  Real-Time Fluid Dynamics 
 *  http://graphics.cs.cmu.edu/nsp/course/15-464/Spring11/papers/StamFluidforGames.pdf
 * 
 **/
#ifndef _RTFD_2D_H
#define _RTFD_2D_H

#include "../../scene/2d/node_2d.h"

class RTFluidDynamics2D : public Node2D
{
    GDCLASS(RTFluidDynamics2D, Node2D);
public:

    RTFluidDynamics2D(/* args */);
    ~RTFluidDynamics2D();

    void test();

protected:
    static void _bind_methods();
};


#endif