#include "RTFluidDynamics2D.h"

#include <cmath>
#include <cstdlib>

RTFluidDynamics2D::RTFluidDynamics2D()
{
    create_grid();
}

RTFluidDynamics2D::~RTFluidDynamics2D()
{
    print_line("Destructor");
    clear_grid();
}

void RTFluidDynamics2D::_notification(int p_what)
{
    switch (p_what)
    {
    case NOTIFICATION_PHYSICS_PROCESS:
        step_simulation();
        break;
    case NOTIFICATION_DRAW:
        if (density != nullptr && render)
        {
            RID ci = get_canvas_item();

            /*density_img->draw_rect(ci, Rect2(Point2(0, 0), Point2(grid_size.x * cell_size.x, grid_size.y * cell_size.y)),
                      false, Color(1, 1, 1, 1)    
            );*/
        }
        break;
    default:
        break;
    }
}

void RTFluidDynamics2D::add_velocity(Vector2i position, Vector2 amount)
{
    int i = IDX(position.x, position.y, grid_size.x);
    x[i] += amount.x;
    y[i] += amount.y;
}

void RTFluidDynamics2D::add_density(Vector2i position, float amount)
{
    density[IDX(position.x, position.y, grid_size.x)] += amount;
}

void RTFluidDynamics2D::create_grid()
{
    s = (float*)calloc(grid_size.x * grid_size.y, sizeof(float));
    density = (float*)calloc(grid_size.x * grid_size.y, sizeof(float));

    x = (float*)calloc(grid_size.x * grid_size.y, sizeof(float));
    y = (float*)calloc(grid_size.x * grid_size.y, sizeof(float));
    p_x = (float*)calloc(grid_size.x * grid_size.y, sizeof(float));
    p_y = (float*)calloc(grid_size.x * grid_size.y, sizeof(float));

    density_img = RenderingServer::get_singleton()->texture_2d_create(density_img);
    density_img->create(grid_size.x * cell_size.x, grid_size.y * cell_size.y, false, Image::Format::FORMAT_RGBA8);
    print_line(itos(density_img->get_width()));
    
}

void RTFluidDynamics2D::clear_grid()
{
    if (s != nullptr)
    {
        delete[] s;
        delete[] density;
        delete[] x;
        delete[] y;
        delete[] p_x;
        delete[] p_y;
    }
}

/* ------------------ Simulation ------------------ */

void RTFluidDynamics2D::step_simulation()
{
    diffuse(1, p_x, x);
    diffuse(2, p_y, y);

    project(p_x, p_y, x, y);

    advect(1, x, p_x, p_x, p_y);
    advect(2, y, p_y, p_x, p_y);

    project(x, y, p_x, p_y);

    diffuse(0, density, s);
    advect(0, density, s, x, y);
}

void RTFluidDynamics2D::diffuse(int b, float* x, float* prev_x)
{
    float a = time_step * diffusion * (grid_size.x - 2) * (grid_size.y - 2);
    linear_solve(b, x, prev_x, a, 6 * a + 1);
}

void RTFluidDynamics2D::project(float* vel_x, float* vel_y, float* p, float* div)
{
    for (int j = 1; j < grid_size.y - 1; j++)
    {
        for (int i = 1; i < grid_size.x - 1; i++)
        {
            div[IDX(i, j, grid_size.x)] = -0.5f * (vel_x[IDX(i + 1, j, grid_size.x)] - vel_x[IDX(i - 1, j, grid_size.x)] + vel_y[IDX(i, j + 1, grid_size.x)] - vel_y[IDX(i, j - 1, grid_size.x)]) / grid_size.x;
            p[IDX(i, j, grid_size.x)] = 0;
        }
    }

    set_bounds(0, div);
    set_bounds(0, p);
    linear_solve(0, p, div, 1, 6);

    for (int j = 1; j < grid_size.y - 1; j++)
    {
        for (int i = 1; i < grid_size.x - 1; i++)
        {
            vel_x[IDX(i, j, grid_size.x)] -= 0.5f * (p[IDX(i + 1, j, grid_size.x)] - p[IDX(i - 1, j, grid_size.x)]) * grid_size.x;
            vel_y[IDX(i, j, grid_size.x)] -= 0.5f * (p[IDX(i, j + 1, grid_size.x)] - p[IDX(i, j - 1, grid_size.x)]) * grid_size.y;
        }
    }
    set_bounds(1, vel_x);
    set_bounds(2, vel_y);
}

void RTFluidDynamics2D::advect(int b, float* d, float* prev_d, float* vel_x, float* vel_y)
{
    float i0, i1, j0, j1;
    
    float dtx = time_step * (grid_size.x - 2);
    float dty = time_step * (grid_size.y - 2);
    
    float s0, s1, t0, t1;
    float tmp1, tmp2, x, y;
    
    float floatx = grid_size.x;
    float floaty = grid_size.y;
    float ifloat, jfloat;

    int i, j;
    
	for(j = 1, jfloat = 1; j < grid_size.y - 1; j++, jfloat++) { 
		for(i = 1, ifloat = 1; i < grid_size.x - 1; i++, ifloat++) {
                	tmp1 = dtx * vel_x[IDX(i, j, grid_size.x)];
                	tmp2 = dty * vel_y[IDX(i, j, grid_size.x)];
                	x = ifloat - tmp1; 
                	y = jfloat - tmp2;
                
                	if(x < 0.5f) x = 0.5f; 
                	if(x > floatx + 0.5f) x = floatx + 0.5f; 
                	i0 = ::floorf(x); 
                	i1 = i0 + 1.0f;
                	if(y < 0.5f) y = 0.5f; 
                	if(y > floaty + 0.5f) y = floaty + 0.5f; 
                	j0 = ::floorf(y);
                	j1 = j0 + 1.0f; 

                	s1 = x - i0; 
                	s0 = 1.0f - s1; 
               		t1 = y - j0; 
                	t0 = 1.0f - t1;
                
                	int i0i = i0;
                	int i1i = i1;
                	int j0i = j0;
                	int j1i = j1;
                
			d[IDX(i, j, grid_size.x )] = 
				s0 * (t0 * prev_d[IDX(i0i, j0i, grid_size.x )] + t1 * prev_d[IDX(i0i, j1i, grid_size.x )]) +
				s1 * (t0 * prev_d[IDX(i1i, j0i, grid_size.x )] + t1 * prev_d[IDX(i1i, j1i, grid_size.x )]);
            	}
        }
	set_bounds(b, d);
}

void RTFluidDynamics2D::linear_solve(int b, float* x, float* prev_x, float a, float c)
{
    float c_recip = 1.0 / c;
    int idx;

    for (int k = 0; k < iterations; k++)
    {
        for (int j = 0; j < grid_size.x; j++)
        {
            for (int i = 0; i < grid_size.y; i++)
            {
                idx = IDX(j, i, grid_size.x);
                x[idx] = (prev_x[idx] + a * 
                        (x[IDX(j, i + 1, grid_size.x)] +
                        x[IDX(j, i - 1, grid_size.x)] +
                        x[IDX(j + 1, i, grid_size.x)] +
                        x[IDX(j - 1, i, grid_size.x)] +
                        x[idx] + x[idx])) * c_recip;
            } 
        }
    }
    set_bounds(b, x);
}

void RTFluidDynamics2D::set_bounds(int b, float* x)
{
    for (int i = 1; i < grid_size.x - 1; i++)
    {
        x[IDX(i, 0, grid_size.x)] = b == 2 ? -x[IDX(i, 1, grid_size.x)] : x[IDX(i, 1, grid_size.x)];
        x[IDX(i, grid_size.x - 1, grid_size.x)] = b == 2 ? -x[IDX(i, grid_size.x - 2, grid_size.x)] : x[IDX(i, grid_size.x - 2, grid_size.x)];
    }

    for (int j = 1; j < grid_size.y - 1; j++)
    {
        x[IDX(0, j, grid_size.x)] = b == 1 ? -x[IDX(1, j, grid_size.x)] : x[IDX(1, j, grid_size.x)];
        x[IDX(grid_size.x - 1, j, grid_size.x)] = b == 1 ? -x[IDX(grid_size.x - 2, j, grid_size.x)] : x[IDX(grid_size.x - 2, j, grid_size.x)];
    }

    float tmp = 0.33f;

    x[IDX(0, 0, grid_size.x)] = tmp * (x[IDX(1, 0, grid_size.x)] + x[IDX(0, 1, grid_size.x)] + x[IDX(0, 0, grid_size.x)]);
    x[IDX(0, grid_size.x - 1, grid_size.x)] = tmp * (x[IDX(1, grid_size.x - 1, grid_size.x)] + x[IDX(0, grid_size.x - 2, grid_size.x)] + x[IDX(0, grid_size.x - 1, grid_size.x)]);
    x[IDX(grid_size.x - 1, 0, grid_size.x)] = tmp * (x[IDX(grid_size.x - 2, 0, grid_size.x)] + x[IDX(grid_size.x - 1, 1, grid_size.x)] + x[IDX(grid_size.x - 1, 0, grid_size.x)]);
    x[IDX(grid_size.x - 1, grid_size.x - 1, grid_size.x)] = tmp * (x[IDX(grid_size.x - 2, grid_size.x - 1, grid_size.x)] + x[IDX(grid_size.x - 1, grid_size.x - 2, grid_size.x)] + x[IDX(grid_size.x - 1, grid_size.x - 1, grid_size.x)]);
}

/* ------------------ Setter & Getter ------------------ */ 

void RTFluidDynamics2D::set_simulating(bool is_simulating)
{
    set_physics_process(is_simulating);
    this->simulating = is_simulating;
}

bool RTFluidDynamics2D::is_simulating()
{
    return this->simulating;
}

void RTFluidDynamics2D::set_grid_size(Vector2i size)
{
    grid_size = size;
    clear_grid();
    create_grid();
}

Vector2 RTFluidDynamics2D::get_grid_size()
{
    return grid_size;
}

void RTFluidDynamics2D::set_diffusion(float diffusion)
{
    this->diffusion = diffusion;
}

float RTFluidDynamics2D::get_diffusion()
{
    return this->diffusion;
}

void RTFluidDynamics2D::set_viscosity(float viscosity)
{
    this->viscosity = viscosity;
}

float RTFluidDynamics2D::get_viscosity()
{
    return this->viscosity;
}

void RTFluidDynamics2D::set_time_step(float time_step)
{
    this->time_step = time_step;
}

float RTFluidDynamics2D::get_time_step()
{
    return time_step;
}

void RTFluidDynamics2D::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_notification", "p_what"), &RTFluidDynamics2D::_notification);

    ClassDB::bind_method(D_METHOD("add_velocity", "position", "amount"), &RTFluidDynamics2D::add_velocity);
    ClassDB::bind_method(D_METHOD("add_density", "position", "amount"), &RTFluidDynamics2D::add_density);

    ClassDB::bind_method(D_METHOD("set_simulating", "is_simulating"), &RTFluidDynamics2D::set_simulating);
    ClassDB::bind_method(D_METHOD("is_simulating"), &RTFluidDynamics2D::is_simulating);
    ClassDB::bind_method(D_METHOD("set_grid_size", "size"), &RTFluidDynamics2D::set_grid_size);
    ClassDB::bind_method(D_METHOD("get_grid_size"), &RTFluidDynamics2D::get_grid_size);

    ClassDB::bind_method(D_METHOD("set_diffusion", "diffusion"), &RTFluidDynamics2D::set_diffusion);
    ClassDB::bind_method(D_METHOD("get_diffusion"), &RTFluidDynamics2D::get_diffusion);
    ClassDB::bind_method(D_METHOD("set_viscosity", "viscosity"), &RTFluidDynamics2D::set_viscosity);
    ClassDB::bind_method(D_METHOD("get_viscosity"), &RTFluidDynamics2D::get_viscosity);
    ClassDB::bind_method(D_METHOD("set_time_step", "time_step"), &RTFluidDynamics2D::set_time_step);
    ClassDB::bind_method(D_METHOD("get_time_step"), &RTFluidDynamics2D::get_time_step);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "grid_size"), "set_grid_size", "get_grid_size");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "simulating"), "set_simulating", "is_simulating");

    ADD_GROUP("Fluid Settings", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "diffusion"), "set_diffusion", "get_diffusion");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "viscosity"), "set_viscosity", "get_viscosity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time_step"), "set_time_step", "get_time_step");
}