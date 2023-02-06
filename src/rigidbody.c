#include <libtarpe2d/rigidbody.h>


void rigidbody_apply_force(struct rigidbody * rb, struct vec2 * force, struct vec2 * apply_point)
{
	vec2_add(&rb->tick_force, force, &rb->tick_force);

	struct vec2 pos_to_apply_point;
	vec2_sub(apply_point, &rb->pos, &pos_to_apply_point);
	rb->torque += vec2_cross_abs(&pos_to_apply_point, force);
}
