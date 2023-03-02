#include <libtarpe2d/bh86/bh.h>
#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/rigidbody.h>
#include <libtarpe2d/tick.h>
#include <stdint.h>


unsigned int tick_rigidbody(struct rb_shape_base * __rbs, void * __dt)
{
	struct rigidbody * rb = (struct rigidbody *)__rbs;
	float_t dt = *((float_t *)__dt);

	struct vec2 tmp;
	vec2_mul(&rb->tick_force, rb->inv_mass * dt, &tmp); // tmp = a * dt

	vec2_add(&rb->linear_velocity, &tmp, &rb->linear_velocity); // v += a * dt

	vec2_mul(&rb->linear_velocity, dt, &tmp); // tmp = v * dt
	vec2_add(&rb->pos, &tmp, &rb->pos); // p =+ v * dt

	float_t angular_acceleration = rb->torque / rb->inv_moment_of_inertia;
	rb->angular_velocity += angular_acceleration * dt;
	rb->angle += rb->angular_velocity * dt;

	vec2_nullify(&rb->tick_force);
	rb->torque = 0;

	return 0;
}

int tarpe_tick(struct rbs_iter * bodies, float_t dt)
{
	int bh86_err;
	if ((bh86_err = bh86_apply_gravity_forces(bodies))) return bh86_err;

	return bodies->iter_func(bodies->data_structure, tick_rigidbody, &dt);
}
