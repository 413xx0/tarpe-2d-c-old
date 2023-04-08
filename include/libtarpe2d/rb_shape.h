#ifndef __LIBTARPE2D_RB_SHAPE_H__
#define __LIBTARPE2D_RB_SHAPE_H__


#include <libtarpe2d/projection.h>
#include <libtarpe2d/rigidbody.h>
#include <libtarpe2d/shape.h>
#include <libtarpe2d/vec2.h>
#include <stdlib.h>
#include <tgmath.h>


struct aabb
{
	float_t x_min, x_max;
	float_t y_min, y_max;
};

static inline void
aabb_init(struct aabb * aabb, struct vec2 * pos, float_t half_width, float_t half_height)
{
	aabb->x_min = pos->x - half_width;
	aabb->x_max = pos->x + half_width;
	aabb->y_min = pos->y - half_height;
	aabb->y_max = pos->y + half_height;
}


struct rb_shape_base
{
	struct rigidbody rb;
	struct aabb aabb;
	enum tarpe_shape_type type;
};


// clang-format off
#define _RB_INIT_ARGS                                                                                    \
	float_t mass, struct vec2 * start_pos, struct vec2 * start_linear_velocity, float_t start_angle, \
		float_t start_angular_velocity
// clang-format on


struct rb_circle
{
	struct rb_shape_base base;
	struct s_circle circle;
};

struct rb_circle * rb_circle_new(float_t radius, _RB_INIT_ARGS);

struct rb_circle * rb_circle_copy(struct rb_circle * circle);

void rb_circle_delete(struct rb_circle * circ);


static inline void
rb_circle_project(struct rb_circle * circ, struct vec2 * normalized_vec, struct projection * proj_out)
{
	float_t center_proj = vec2_dot(&(circ->base.rb.pos), normalized_vec);
	*proj_out = (struct projection){
		.min = center_proj - circ->circle.radius, .max = center_proj + circ->circle.radius};
}


struct rb_rectangle
{
	struct rb_shape_base base;
	struct s_rectangle rect;
	struct vec2 vertices[4];
	struct vec2 normals[4];
};

struct rb_rectangle * rb_rectangle_new(float_t width, float_t height, _RB_INIT_ARGS);

struct rb_rectangle * rb_rectangle_copy(struct rb_rectangle * rect);

void rb_rectangle_delete(struct rb_rectangle * rect);


void rb_rectangle_update_vertices(struct rb_rectangle * rect);

void rb_rectangle_update_normals(struct rb_rectangle * rect);

static inline void rb_rectangle_project(struct rb_rectangle * rect, struct vec2 * normalized_vec,
					struct projection * proj_out)
{
	float_t center_proj = vec2_dot(&(rect->base.rb.pos), normalized_vec);
	float_t delta =
		rect->rect.half_side_sizes.x * fabs(vec2_dot(rect->normals + 1, normalized_vec))
		+ rect->rect.half_side_sizes.y * fabs(vec2_dot(rect->normals + 2, normalized_vec));
	*proj_out = (struct projection){.min = center_proj - delta, .max = center_proj + delta};
}


void rb_shape_delete(struct rb_shape_base * shape);


#endif /*__LIBTARPE2D_RB_SHAPE_H__*/
