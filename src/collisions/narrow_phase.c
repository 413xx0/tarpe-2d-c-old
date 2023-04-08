#include <libtarpe2d/collisions/narrow_phase.h>
#include <libtarpe2d/math_utils.h>
#include <libtarpe2d/projection.h>
#include <tgmath.h>


static inline bool get_circle_circle_penetration(struct rb_circle * circ_1, struct rb_circle * circ_2,
						 struct vec2 * penetration_out)
{
	struct vec2 dist;
	vec2_sub(&(circ_2->base.rb.pos), &(circ_1->base.rb.pos), &dist);
	float_t dist_abs = vec2_abs(&dist);
	float_t depth = (circ_1->circle.radius + circ_2->circle.radius) - dist_abs;
	if (depth > 0)
	{
		vec2_div(&dist, dist_abs, &dist);
		vec2_mul(&dist, depth, penetration_out);
		return true;
	}
	return false;
}

static inline float_t get_mtv_abs(struct projection * proj_1, struct projection * proj_2,
				  float_t projs_overlap, struct vec2 * axis_normal)
{
	if (projection_is_contained_exclusive(proj_1, proj_2)
	    || projection_is_contained_exclusive(proj_2, proj_1))
	{
		float_t max_diff = fabs(proj_1->max - proj_2->max),
			min_diff = fabs(proj_1->min - proj_2->min);
		if (max_diff > min_diff)
		{
			vec2_neg(axis_normal, axis_normal);
			return projs_overlap + min_diff;
		}
		return projs_overlap + max_diff;
	}
	return projs_overlap;
}

static inline void set_mtv_from_1_to_2(struct rb_shape_base * shape_1, struct rb_shape_base * shape_2,
				       struct vec2 * axis_normal)
{
	struct vec2 center_to_center;
	vec2_sub(&(shape_2->rb.pos), &(shape_1->rb.pos), &center_to_center);
	if (vec2_dot(&center_to_center, axis_normal) < 0) vec2_neg(axis_normal, axis_normal);
}

#define CHECK_AXIS(axis_normal_ptr,                                                          \
		   shape_1,                                                                  \
		   proj_func_1,                                                              \
		   proj_1,                                                                   \
		   shape_2,                                                                  \
		   proj_func_2,                                                              \
		   proj_2,                                                                   \
		   axis_overlap,                                                             \
		   min_overlap,                                                              \
		   min_axis_normal_ptr)                                                      \
	do {                                                                                 \
		proj_func_1(shape_1, axis_normal_ptr, &proj_1);                              \
		proj_func_2(shape_2, axis_normal_ptr, &proj_2);                              \
                                                                                             \
		axis_overlap = projection_get_overlap(&proj_1, &proj_2);                     \
		if (axis_overlap == 0) return false;                                         \
		axis_overlap = get_mtv_abs(&proj_1, &proj_2, axis_overlap, axis_normal_ptr); \
                                                                                             \
		if (axis_overlap < min_overlap)                                              \
		{                                                                            \
			min_overlap = axis_overlap;                                          \
			min_axis_normal_ptr = axis_normal_ptr;                               \
		}                                                                            \
	} while (0);

static inline void get_circle_rect_third_axis(struct rb_circle * circ, struct rb_rectangle * rect,
					      struct vec2 * axis_normal_out)
{
	struct vec2 * circ_center = &(circ->base.rb.pos);
	struct vec2 * closest = rect->vertices;
	float_t shortest_sqr_dist = vec2_sqr_dist(closest, circ_center), cur_sqr_dist;
	for (struct vec2 * vert = rect->vertices + 1; vert < rect->vertices + 4; ++vert)
	{
		cur_sqr_dist = vec2_sqr_dist(vert, circ_center);
		if (cur_sqr_dist < shortest_sqr_dist)
		{
			closest = vert;
			shortest_sqr_dist = cur_sqr_dist;
		}
	}

	vec2_sub(closest, circ_center, axis_normal_out);
	vec2_norm(axis_normal_out, axis_normal_out);
}

static inline bool get_circle_rect_penetration(struct rb_circle * circ, struct rb_rectangle * rect,
					       struct vec2 * penetration_out)
{
	struct vec2 * axis_normal = NULL;
	float_t overlap = FLT_T_MAX, cur_overlap;
	struct projection proj_1, proj_2;

#define CR_CHECK_AXIS                    \
	CHECK_AXIS(cur_normal,           \
		   circ,                 \
		   rb_circle_project,    \
		   proj_1,               \
		   rect,                 \
		   rb_rectangle_project, \
		   proj_2,               \
		   cur_overlap,          \
		   overlap,              \
		   axis_normal);

	struct vec2 * cur_normal = rect->normals + 1;
	for (; cur_normal < rect->normals + 3; ++cur_normal)
	{
		if (vec2_is_zero(cur_normal)) continue;
		CR_CHECK_AXIS;
	}

	get_circle_rect_third_axis(circ, rect, cur_normal);
	CR_CHECK_AXIS;

	if (axis_normal == NULL) return false;

	set_mtv_from_1_to_2(&(circ->base), &(rect->base), axis_normal);

	vec2_mul(axis_normal, overlap, penetration_out);
	return true;
}

static inline bool get_rect_rect_penetration(struct rb_rectangle * rect_1, struct rb_rectangle * rect_2,
					     struct vec2 * penetration_out)
{
	struct vec2 * axis_normal = NULL;
	float_t overlap = FLT_T_MAX, cur_overlap;
	struct projection proj_1, proj_2;

#define RR_CHECK_AXES_LOOP_BODY                 \
	if (vec2_is_zero(cur_normal)) continue; \
	CHECK_AXIS(cur_normal,                  \
		   rect_1,                      \
		   rb_rectangle_project,        \
		   proj_1,                      \
		   rect_2,                      \
		   rb_rectangle_project,        \
		   proj_2,                      \
		   cur_overlap,                 \
		   overlap,                     \
		   axis_normal);

	for (struct vec2 * cur_normal = rect_1->normals + 1; cur_normal < rect_1->normals + 3;
	     ++cur_normal)
	{
		RR_CHECK_AXES_LOOP_BODY;
	}

	for (struct vec2 * cur_normal = rect_2->normals + 1; cur_normal < rect_2->normals + 3;
	     ++cur_normal)
	{
		RR_CHECK_AXES_LOOP_BODY;
	}

	if (axis_normal == NULL) return false;

	set_mtv_from_1_to_2(&(rect_1->base), &(rect_1->base), axis_normal);

	vec2_mul(axis_normal, overlap, penetration_out);
	return true;
}


bool get_penetration(struct rb_shape_base * body_1, struct rb_shape_base * body_2,
		     struct vec2 * penetration_out)
{
	if (body_1->type == TARPE__SHAPE_TYPE__CIRCLE)
	{
		if (body_2->type == TARPE__SHAPE_TYPE__CIRCLE)
			return get_circle_circle_penetration(
				(struct rb_circle *)body_1, (struct rb_circle *)body_2, penetration_out);
		else /* body_2->type == TARPE__SHAPE_TYPE__RECTANGLE */
			return get_circle_rect_penetration((struct rb_circle *)body_1,
							   (struct rb_rectangle *)body_2,
							   penetration_out);
	}
	else /* body_1->type == TARPE__SHAPE_TYPE__RECTANGLE */
	{
		if (body_2->type == TARPE__SHAPE_TYPE__CIRCLE)
		{
			bool res = get_circle_rect_penetration((struct rb_circle *)body_2,
							       (struct rb_rectangle *)body_1,
							       penetration_out);
			if (res) vec2_neg(penetration_out, penetration_out);
			return res;
		}
		else /* body_2->type == TARPE__SHAPE_TYPE__RECTANGLE */
		{
			return get_rect_rect_penetration((struct rb_rectangle *)body_1,
							 (struct rb_rectangle *)body_2,
							 penetration_out);
		}
	}
}
