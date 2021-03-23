/**
 * 
 *  Real-Time Fluid Dynamics 
 *  http://graphics.cs.cmu.edu/nsp/course/15-464/Spring11/papers/StamFluidforGames.pdf
 * 
 **/
#ifndef _RTFD_2D_H
#define _RTFD_2D_H

#include "scene/2d/node_2d.h"
#include "core/math/vector2.h"

#define IDX(x, y, grid_x) ((y * grid_x) + x)

class RTFluidDynamics2D : public Node2D
{
    GDCLASS(RTFluidDynamics2D, Node2D);
public:

    RTFluidDynamics2D();
    ~RTFluidDynamics2D();

    void _notification(int p_what);

    void add_velocity(Vector2i position, Vector2 amount);
    void add_density(Vector2i position, float amount);

    void set_simulating(bool is_simulating);
    bool is_simulating();
    void set_grid_size(Vector2i size);
    Vector2 get_grid_size();

    void set_diffusion(float diffusion);
    float get_diffusion();
    void set_viscosity(float viscosity);
    float get_viscosity();
    void set_time_step(float time_step);
    float get_time_step();

public:
    bool simulating = false;
    bool render = false;
    Vector2i grid_size = Vector2i(10, 10);
    Vector2i cell_size = Vector2i(10, 10);

    // Fluid Settings
    int iterations = 8;
    float diffusion = 0.0f;
    float viscosity = 0.0000001f;
    float time_step = 0.2f;

    Ref<Image> density_img;

protected:
    static void _bind_methods();

private:
    float* s;
    float* density;

    float* x;
    float* y;
    float* p_x;
    float* p_y;

    void step_simulation();

    void diffuse(int b, float* x, float* prev_x);
    void project(float* vel_x, float* vel_y, float* p, float* div);
    void advect(int b, float* d, float* prev_d, float* vel_x, float* vel_y);

    void set_bounds(int b, float* x);
    void linear_solve(int b, float* x, float* prev_x, float a, float c);

    void create_grid();
    void clear_grid();

    // void render();
};


#endif