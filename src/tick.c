#include <libtarpe2d/bh86/bh.h>
#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/rigidbody.h>
#include <libtarpe2d/tick.h>
#include <stdint.h>


static inline void tick_rigidbody(struct rigidbody * rb, double_t dt)
{
	struct vec2 tmp;
	vec2_mul(&rb->tick_force, rb->inv_mass * dt, &tmp); // tmp = a * dt

	vec2_add(&rb->linear_velocity, &tmp, &rb->linear_velocity); // v += a * dt

	vec2_mul(&rb->linear_velocity, dt, &tmp); // tmp = v * dt
	vec2_add(&rb->pos, &tmp, &rb->pos); // p =+ v * dt

	double_t angular_acceleration = rb->torque / rb->inv_moment_of_inertia;
	rb->angular_velocity += angular_acceleration * dt;
	rb->angle += rb->angular_velocity * dt;

	vec2_nullify(&rb->tick_force);
	rb->torque = 0;
}

int tarpe_tick_ptr_arr_iter(struct rb_ptr_array_iter * iter, double_t dt)
{
	if (bh86_apply_gravity_forces_ptr_arr_iter(iter)) return 1;

	for (struct rb_shape_base ** i = iter->start; i < iter->end;
	     i = (struct rb_shape_base **)((int8_t *)i + iter->step))
	{
		tick_rigidbody(&((*i)->rb), dt);
	}
	return 0;
}

int tarpe_tick_uni_iter(struct rb_uni_iter * iter, double_t dt)
{
	if (bh86_apply_gravity_forces_uni_iter(iter)) return 1;

	void * userdata = malloc(iter->userdata_size);
	if (userdata == NULL) return 1;
	memset(userdata, 0, iter->userdata_size);

	struct rb_shape_base * i = iter->get_first(iter->data_structure, userdata);
	while (i != NULL)
	{
		tick_rigidbody(&(i->rb), dt);
		i = iter->get_next(userdata);
	}
	return 0;
}
