#ifndef __LIBTARPE2d_RIGIDBODY_H__
#define __LIBTARPE2d_RIGIDBODY_H__


#include <libtarpe2d/vec2.h>
#include <tgmath.h>


struct rigidbody
{
	double_t mass;
	double_t inv_mass;
	double_t inv_moment_of_inertia;

	struct vec2 pos;

	struct vec2 linear_velocity;

	double_t angle;
	double_t angular_velocity;

	struct vec2 tick_force;
	double_t torque;
};


void rigidbody_apply_force(struct rigidbody * rb, struct vec2 * force, struct vec2 * apply_point);


#endif /*__LIBTARPE2d_RIGIDBODY_H__*/
