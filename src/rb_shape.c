#include <libtarpe2d/math_utils.h>
#include <libtarpe2d/rb_shape.h>
#include <string.h>
#include <tgmath.h>


#define _SET_RB_INIT_ARGS(rb)                                          \
	do {                                                           \
		rb.mass = mass;                                        \
		rb.inv_mass = 1 / mass;                                \
		rb.friction = friction;                                \
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
		_SET_RB_INIT_ARGS(circ->base.rb);
		circ->circle.radius = radius;

		circ->base.rb.inv_moment_of_inertia =
			2 / (circ->base.rb.mass * flt_squared(circ->circle.radius));
		circ->base.type = TARPE__SHAPE_TYPE__CIRCLE;

		aabb_init(&(circ->base.aabb), start_pos, radius, radius);
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
		_SET_RB_INIT_ARGS(rect->base.rb);
		rect->base.rb.inv_moment_of_inertia =
			12 / (rect->base.rb.mass * (flt_squared(width) + flt_squared(height)));
		rect->base.type = TARPE__SHAPE_TYPE__RECTANGLE;

		rect->rect.half_side_sizes = (struct vec2){.x = width / 2, .y = height / 2};

		float_t aabb_side = vec2_abs(&rect->rect.half_side_sizes);
		aabb_init(&(rect->base.aabb), start_pos, aabb_side, aabb_side);

		rb_rectangle_update_vertices(rect);
		rb_rectangle_update_normals(rect);
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

void rb_rectangle_update_vertices(struct rb_rectangle * rect)
{
	float_t sin_rot = sin(rect->base.rb.angle);
	float_t cos_rot = cos(rect->base.rb.angle);
	struct vec2 rot_half_width = (struct vec2){.x = rect->rect.half_side_sizes.x * cos_rot,
						   .y = rect->rect.half_side_sizes.x * sin_rot};
	struct vec2 rot_half_height = (struct vec2){.x = rect->rect.half_side_sizes.y * -sin_rot,
						    .y = rect->rect.half_side_sizes.y * cos_rot};

	struct vec2 right, left;
	vec2_add(&rect->base.rb.pos, &rot_half_width, &right);
	vec2_sub(&rect->base.rb.pos, &rot_half_width, &left);

	vec2_add(&left, &rot_half_height, rect->vertices);
	vec2_add(&right, &rot_half_height, rect->vertices + 1);
	vec2_sub(&right, &rot_half_height, rect->vertices + 2);
	vec2_sub(&left, &rot_half_height, rect->vertices + 3);
}

static inline void get_left_normal(struct vec2 * vec, struct vec2 * out_normal)
{
	*out_normal = (struct vec2){.x = vec->y, .y = -vec->x};
	vec2_norm(out_normal, out_normal);
}

void rb_rectangle_update_normals(struct rb_rectangle * rect)
{
	struct vec2 side;

	vec2_sub(rect->vertices + 1, rect->vertices, &side);
	get_left_normal(&side, rect->normals);

	vec2_sub(rect->vertices + 2, rect->vertices + 1, &side);
	get_left_normal(&side, rect->normals + 1);

	vec2_neg(rect->normals, rect->normals + 2);
	vec2_neg(rect->normals + 1, rect->normals + 3);
}


void rb_shape_delete(struct rb_shape_base * shape) { free(shape); }


void rb_shape_update_aabb(struct rb_shape_base * shape)
{
	if (shape->type == TARPE__SHAPE_TYPE__CIRCLE)
	{
		float_t radius = ((struct rb_circle *)shape)->circle.radius;
		aabb_init(&(shape->aabb), &(shape->rb.pos), radius, radius);
	}
	else
	{
		float_t aabb_side = vec2_abs(&(((struct rb_rectangle *)shape)->rect.half_side_sizes));
		aabb_init(&(shape->aabb), &(shape->rb.pos), aabb_side, aabb_side);
	}
}
