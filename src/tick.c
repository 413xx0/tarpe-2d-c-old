#include <libtarpe2d/bh86/bh.h>
#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/rigidbody.h>
#include <libtarpe2d/tick.h>
#include <stdint.h>


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

	vec2_nullify(&rb->tick_force);
	rb->torque = 0;
}

int tarpe_tick(struct rbs_array * bodies, float_t dt)
{
	int bh86_err;
	if ((bh86_err = bh86_apply_gravity_forces(bodies))) return bh86_err;

	for (struct rb_shape_base ** rbs_ptr = bodies->shapes; rbs_ptr < bodies->shapes + bodies->size; ++rbs_ptr)
	{
		tick_rigidbody((struct rigidbody *)(*rbs_ptr), dt);
	}

	return 0;
}
