#include <libtarpe2d/rigidbody.h>
#include <libtarpe2d/tick.h>


static inline void tick_rigidbody(struct rigidbody * rb, float_t dt)
{
	struct vec2 tmp;
	vec2_mul(&rb->tick_force, rb->inv_mass * dt, &tmp); // tmp = a * dt

	vec2_add(&rb->linear_velocity, &tmp, &rb->linear_velocity); // v += a * dt

	vec2_mul(&rb->linear_velocity, dt, &tmp); // tmp = v * dt
	vec2_add(&rb->pos, &tmp, &rb->pos); // p =+ v * dt

	float_t angular_acceleration = rb->torque / rb->inv_moment_of_inertia;
	rb->angular_velocity += angular_acceleration * dt;
	rb->angle += rb->angular_velocity * dt;
}


void tarpe_tick_arr_iter(struct rb_array_iter * iter, float_t dt)
{
	for (struct rigidbody * i = iter->start; i < iter->end; i += iter->step)
	{
		tick_rigidbody(i, dt);
	}
}

int tarpe_tick_uni_iter(struct rb_uni_iter * iter, float_t dt)
{
	void * userdata = malloc(iter->userdata_size);
	if (userdata == NULL) return 1;

	struct rigidbody * i = iter->get_first(iter->data_structure, userdata);
	while (i != NULL)
	{
		tick_rigidbody(i, dt);
		i = iter->get_next(userdata);
	}
	return 0;
}
