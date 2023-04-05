#include <libtarpe2d/collisions/broad_phase.h>
#include <libtarpe2d/collisions/narrow_phase.h>
#include <libtarpe2d/collisions/solve_collision.h>
#include <libtarpe2d/math_utils.h>
#include <libtarpe2d/rb_shape.h>
#include <stdlib.h>
#include <string.h>


static inline int is_colliding_aabb_aabb_x(struct rb_shape_base * body_1, struct rb_shape_base * body_2)
{
	return body_1->aabb.x_min <= body_2->aabb.x_max && body_2->aabb.x_min <= body_1->aabb.x_max;
}

static inline int is_colliding_aabb_aabb_y(struct rb_shape_base * body_1, struct rb_shape_base * body_2)
{
	return body_1->aabb.y_min <= body_2->aabb.y_max && body_2->aabb.y_min <= body_1->aabb.y_max;
}


// 0 - x, 1 - y
static inline int find_axis(struct rbs_array * bodies)
{
	struct vec2 sum = {0, 0}, squared_sum = {0, 0}, center;
	for (struct rb_shape_base ** iter = bodies->shapes; iter < bodies->shapes + bodies->size; ++iter)
	{
		center = (*iter)->rb.pos;
		vec2_add(&sum, &center, &sum);
		squared_sum.x += flt_squared(center.x);
		squared_sum.y += flt_squared(center.y);
	}
	struct vec2 variance;
	sum.x *= sum.x;
	sum.y *= sum.y;
	vec2_sub(&squared_sum, &sum, &variance);
	return variance.y > variance.x;
}

static int _cmp_bodies_x(const void * __body_1, const void * __body_2)
{
	const struct rb_shape_base *body_1 = *((struct rb_shape_base **)__body_1),
				   *body_2 = *((struct rb_shape_base **)__body_2);

	return body_1->aabb.x_min - body_2->aabb.x_min;
}

static int _cmp_bodies_y(const void * __body_1, const void * __body_2)
{
	const struct rb_shape_base *body_1 = *((struct rb_shape_base **)__body_1),
				   *body_2 = *((struct rb_shape_base **)__body_2);

	return body_1->aabb.y_min - body_2->aabb.y_min;
}

int sap_cd_broad_phase(struct rbs_array * bodies)
{
	struct rb_shape_base ** sorted = malloc(sizeof(struct rb_shape_base *) * bodies->size);
	if (sorted == NULL) return 1;
	memcpy(sorted, bodies->shapes, sizeof(struct rb_shape_base *) * bodies->size);

#define LOOP_THROUGH_PAIRS                                                                  \
	for (struct rb_shape_base ** main_iter = sorted; main_iter < sorted + bodies->size; \
	     ++main_iter)                                                                   \
	{                                                                                   \
		for (struct rb_shape_base ** sub_iter = main_iter + 1;                      \
		     sub_iter < sorted + bodies->size;                                      \
		     ++sub_iter)                                                            \
		{
#define CLOSE_LOOP \
	}          \
	}

#define PROCESS_POSSIBLE_COLLISION                                    \
	if (get_penetration(*sub_iter, *main_iter, &penetration))     \
	{                                                             \
		solve_collision(*sub_iter, *main_iter, &penetration); \
	}

	int sort_axis = find_axis(bodies);
	struct vec2 penetration;
	if (sort_axis == 1) /* y */
	{
		qsort(sorted, bodies->size, sizeof(struct rb_shape_base *), _cmp_bodies_y);

		LOOP_THROUGH_PAIRS
		if ((*sub_iter)->aabb.y_min > (*main_iter)->aabb.y_max) break;
		if (is_colliding_aabb_aabb_x(*sub_iter, *main_iter)) PROCESS_POSSIBLE_COLLISION;
		CLOSE_LOOP
	}
	else /* x */
	{
		qsort(sorted, bodies->size, sizeof(struct rb_shape_base *), _cmp_bodies_x);

		LOOP_THROUGH_PAIRS
		if ((*sub_iter)->aabb.x_min > (*main_iter)->aabb.x_max) break;
		if (is_colliding_aabb_aabb_y(*sub_iter, *main_iter)) PROCESS_POSSIBLE_COLLISION;
		CLOSE_LOOP
	}

	return 0;
}
