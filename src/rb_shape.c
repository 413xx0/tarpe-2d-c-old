#include <libtarpe2d/math_utils.h>
#include <libtarpe2d/rb_shape.h>
#include <string.h>


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
		circ->circle.radius = radius;
		circ->base.type = TARPE__SHAPE_TYPE__CIRCLE;

		_SET_RB_INIT_ARGS(circ->base.rb);
		circ->base.rb.inv_moment_of_inertia =
			2 / (circ->base.rb.mass * flt_squared(circ->circle.radius));
	}
	return circ;
}

struct rb_circle * rb_circle_copy(struct rb_circle * src)
{
	struct rb_circle * circ = malloc(sizeof(struct rb_circle));
	if (circ != NULL) memcpy(circ, src, sizeof(struct rb_circle));
	return circ;
}

void rb_circle_delete(struct rb_circle * circ) { free(circ); }


struct rb_rectangle * rb_rectangle_new(float_t width, float_t height, _RB_INIT_ARGS)
{
	struct rb_rectangle * rect = malloc(sizeof(struct rb_rectangle));
	if (rect != NULL)
	{
		// rect->rect.side_sizes = (struct vec2){.x = width, .y = height};
		rect->rect.half_side_sizes = (struct vec2){.x = width / 2, .y = height / 2};
		rect->base.type = TARPE__SHAPE_TYPE__RECTANGLE;

		_SET_RB_INIT_ARGS(rect->base.rb);
		rect->base.rb.inv_moment_of_inertia =
			12 / (rect->base.rb.mass * (flt_squared(width) + flt_squared(height)));
	}
	return rect;
}

struct rb_rectangle * rb_rectangle_copy(struct rb_rectangle * src)
{
	struct rb_rectangle * rect = malloc(sizeof(struct rb_rectangle));
	if (rect != NULL) memcpy(rect, src, sizeof(struct rb_rectangle));
	return rect;
}

void rb_rectangle_delete(struct rb_rectangle * rect) { free(rect); }


void rb_shape_delete(struct rb_shape_base * shape) { free(shape); }
