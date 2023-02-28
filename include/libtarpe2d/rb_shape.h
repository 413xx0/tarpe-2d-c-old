#ifndef __LIBTARPE2d_SHAPE_H__
#define __LIBTARPE2d_SHAPE_H__


#include <libtarpe2d/rigidbody.h>
#include <libtarpe2d/vec2.h>
#include <stdlib.h>
#include <tgmath.h>


enum rb_shape_type
{
	TARPE__RB_SHAPE_TYPE__CIRCLE,
	TARPE__RB_SHAPE_TYPE__RECTANGLE
};


struct rb_shape_base
{
	struct rigidbody rb;
	enum rb_shape_type type;
};


// clang-format off
#define _RB_INIT_ARGS                                                                                    \
	float_t mass, struct vec2 * start_pos, struct vec2 * start_linear_velocity, float_t start_angle, \
		float_t start_angular_velocity
// clang-format on


struct rb_circle
{
	struct rb_shape_base base;
	float_t radius;
};

struct rb_circle * rb_circle_new(float_t radius, _RB_INIT_ARGS);

struct rb_circle * rb_circle_copy(struct rb_circle * circle);

void rb_circle_delete(struct rb_circle * circ);


struct rb_rectangle
{
	struct rb_shape_base base;
	struct vec2 side_sizes;
	struct vec2 half_side_sizes;
};

struct rb_rectangle * rb_rectangle_new(float_t width, float_t height, _RB_INIT_ARGS);

struct rb_rectangle * rb_rectangle_copy(struct rb_rectangle * rect);

void rb_rectangle_delete(struct rb_rectangle * rect);


void rb_shape_delete(struct rb_shape_base * shape);


#endif /*__LIBTARPE2d_SHAPE_H__*/
