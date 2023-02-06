#include <libtarpe2d/rb_shape.h>


#define _SET_RB_INIT_ARGS(rb)                                          \
	do {                                                           \
		rb.mass = mass;                                        \
		rb.inv_mass = 1 / mass;                                \
		vec2_copy(&rb.pos, start_pos);                         \
		vec2_copy(&rb.linear_velocity, start_linear_velocity); \
		rb.angle = start_angle;                                \
		rb.angular_velocity = start_angular_velocity;          \
	} while (0);


struct rb_circle * rb_circle_new(float_t radius, _RB_INIT_ARGS)
{
	struct rb_circle * circ = malloc(sizeof(struct rb_circle));
	if (circ != NULL)
	{
		circ->radius = radius;
		circ->base.type = TARPE__RB_SHAPE_TYPE__CIRCLE;

		_SET_RB_INIT_ARGS(circ->base.rb);
		circ->base.rb.inv_moment_of_inertia =
			2 / (circ->base.rb.mass * circ->radius * circ->radius);
	}
	return circ;
}

void rb_circle_delete(struct rb_circle * circ) { free(circ); }


struct rb_rectangle * rb_rectangle_new(float_t width, float_t height, _RB_INIT_ARGS)
{
	struct rb_rectangle * rect = malloc(sizeof(struct rb_rectangle));
	if (rect != NULL)
	{
		rect->side_sizes = (struct vec2){.x = width, .y = height};
		rect->base.type = TARPE__RB_SHAPE_TYPE__RECTANGLE;

		_SET_RB_INIT_ARGS(rect->base.rb);
		rect->base.rb.inv_moment_of_inertia = 12
						      / (rect->base.rb.mass
							 * (rect->side_sizes.x * rect->side_sizes.x
							    + rect->side_sizes.y * rect->side_sizes.y));
	}
	return rect;
}

void rb_rectangle_delete(struct rb_rectangle * rect) { free(rect); }
