#include <libtarpe2d/collisions/find_contact_points.h>
#include <stdbool.h>


static inline void
circle_get_best_point(struct rb_circle * circ, struct vec2 * normal, struct vec2 * point_out)
{
	vec2_mul(normal, circ->circle.radius, point_out);
	vec2_add(point_out, &(circ->base.rb.pos), point_out);
}


struct edge_feature
{
	struct vec2 max;
	struct vec2 vertex_1, vertex_2;
	struct vec2 edge;
};

static inline void
rect_get_best_edge(struct rb_rectangle * rect, struct vec2 * normal, struct edge_feature * edge_out)
{
	int max_idx = 0;
	float_t max_proj = vec2_dot(rect->vertices, normal);
	float_t tmp_proj;
	for (int i = 1; i < 4; ++i)
	{
		tmp_proj = vec2_dot(rect->vertices + i, normal);
		if (tmp_proj > max_proj)
		{
			max_proj = tmp_proj;
			max_idx = i;
		}
	}

	struct vec2 *max_vert = rect->vertices + max_idx,
		    *prev_vert = rect->vertices + (max_idx - 1 == -1 ? 3 : max_idx - 1),
		    *next_vert = rect->vertices + (max_idx + 1 == 4 ? 0 : max_idx + 1);

	struct vec2 left, right;
	vec2_sub(max_vert, prev_vert, &left);
	vec2_norm(&left, &left);
	vec2_sub(max_vert, next_vert, &right);
	vec2_norm(&right, &right);

	edge_out->max = *max_vert;
	if (vec2_dot(&right, normal) <= vec2_dot(&left, normal))
	{
		edge_out->edge = right;
		edge_out->vertex_1 = *max_vert;
		edge_out->vertex_2 = *next_vert;
	}
	else
	{
		edge_out->edge = left;
		edge_out->vertex_1 = *prev_vert;
		edge_out->vertex_2 = *max_vert;
	}
}


static inline int clip(struct vec2 * point_1, struct vec2 * point_2, struct vec2 * normal,
		       float_t min_dot, struct vec2 clipped_points[2])
{
	int cnt = 0;

	float_t delta_1 = vec2_dot(point_1, normal) - min_dot,
		delta_2 = vec2_dot(point_2, normal) - min_dot;

	if (delta_1 >= 0) clipped_points[cnt++] = *point_1;
	if (delta_2 >= 0) clipped_points[cnt++] = *point_2;

	if (delta_1 * delta_2 < 0)
	{
		struct vec2 shifted_point;
		vec2_sub(point_2, point_1, &shifted_point);
		vec2_mul(&shifted_point, delta_1 / (delta_1 - delta_2), &shifted_point);
		vec2_add(&shifted_point, point_1, clipped_points + cnt);
		++cnt;
	}

	return cnt;
}


void find_contact_points(struct manifold * manifold)
{
	struct vec2 neg_normal, normal = manifold->normal;
	vec2_neg(&(manifold->normal), &neg_normal);

	if (manifold->body_1->type == TARPE__SHAPE_TYPE__CIRCLE)
	{
		circle_get_best_point((struct rb_circle *)manifold->body_1,
				      &(manifold->normal),
				      manifold->contact_points);
		manifold->contact_points_count = 1;
		vec2_copy(&(manifold->normal), &neg_normal);
		return;
	}

	if (manifold->body_2->type == TARPE__SHAPE_TYPE__CIRCLE)
	{
		circle_get_best_point(
			(struct rb_circle *)manifold->body_2, &neg_normal, manifold->contact_points);
		manifold->contact_points_count = 1;
		vec2_copy(&(manifold->normal), &neg_normal);
		return;
	}

	manifold->contact_points_count = 0;

	struct edge_feature ref, inc;
	rect_get_best_edge((struct rb_rectangle *)manifold->body_1, &normal, &ref);
	rect_get_best_edge((struct rb_rectangle *)manifold->body_2, &neg_normal, &inc);

	bool is_flipped = false;
	if (fabs(vec2_dot(&(ref.edge), &normal)) > fabs(vec2_dot(&(inc.edge), &normal)))
	{
		struct edge_feature ef = ref;
		ref = inc;
		inc = ef;
		is_flipped = true;
	}

	struct vec2 ref_unit, neg_ref_unit;
	vec2_norm(&(ref.edge), &ref_unit);
	vec2_neg(&ref_unit, &neg_ref_unit);

	struct vec2 cp_first[2];
	if (clip(&(inc.vertex_1),
		 &(inc.vertex_2),
		 &neg_ref_unit,
		 -vec2_dot(&ref_unit, &(ref.vertex_1)),
		 cp_first)
	    < 2)
		return;

	struct vec2 cp_second[2];
	if (clip(cp_first, cp_first + 1, &ref_unit, vec2_dot(&ref_unit, &(ref.vertex_2)), cp_second)
	    < 2)
		return;
	
	struct vec2 ref_normal = {.x = -ref.edge.y, .y = ref.edge.x};
	if (is_flipped) vec2_neg(&ref_normal, &(manifold->normal));

	float_t max_depth = vec2_dot(&ref_normal, &(ref.max));

	if (vec2_dot(&ref_normal, cp_second) - max_depth >= 0)
		manifold->contact_points[manifold->contact_points_count++] = cp_second[0];
	if (vec2_dot(&ref_normal, cp_second + 1) - max_depth >= 0)
		manifold->contact_points[manifold->contact_points_count++] = cp_second[1];
	
	// manifold->normal = ref_normal;
}
