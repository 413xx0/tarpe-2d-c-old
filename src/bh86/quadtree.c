#include <libtarpe2d/bh86/quadtree.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


static inline size_t quadtree_quarter(struct quadtree * qt, struct vec2 * body_pos)
{
	size_t idx = 0;
	if (qt->pos.x <= body_pos->x) idx += 2;
	if (qt->pos.y <= body_pos->y) idx += 1;
	return idx;
}

static inline int quadtree_divide(struct quadtree * qt)
{
	qt->childs = malloc(sizeof(struct quadtree) * 4);
	if (qt->childs == NULL) return 1;

	float_t half_size = qt->size / 2;
	float_t dp_x = -half_size / 2, dp_y = dp_x;
	for (size_t i = 0; i < 4; ++i)
	{
		qt->childs[i].pos.x = qt->pos.x + dp_x;
		qt->childs[i].pos.y = qt->pos.y + dp_y;
		qt->childs[i].size = half_size;
		qt->childs[i].state = _BH86_QTSTATE_EMPTY;
		memset((((int8_t *)(qt->childs + i)) + offsetof(struct quadtree, total_mass)),
		       0,
		       sizeof(struct quadtree) - offsetof(struct quadtree, total_mass));

		dp_y *= -1;
		if (i == 1) dp_x *= -1;
	}

	return 0;
}

static inline void quadtree_empty_insert(struct quadtree * qt, struct rigidbody * rb)
{
	qt->rb = rb;
	vec2_mul(&rb->pos, rb->mass, &qt->center_of_mass);
	qt->total_mass = rb->mass;
	qt->state = _BH86_QTSTATE_FULL;
}

int quadtree_insert(struct quadtree * qt, struct rigidbody * rb)
{
	if (qt->state & _BH86_QTSTATE_EMPTY) { quadtree_empty_insert(qt, rb); }
	else
	{
		if (qt->state & _BH86_QTSTATE_FULL)
		{
			if (quadtree_divide(qt)) return 1;
			quadtree_empty_insert(qt->childs + quadtree_quarter(qt, &qt->rb->pos), qt->rb);
			qt->state = _BH86_QTSTATE_DIVIDED;
		}
		quadtree_insert(qt->childs + quadtree_quarter(qt, &rb->pos), rb);

		struct vec2 tmp;
		vec2_mul(&rb->pos, rb->mass, &tmp);
		vec2_add(&qt->center_of_mass, &tmp, &qt->center_of_mass);
		qt->total_mass += rb->mass;
	}

	return 0;
}


void quadtree_compute_centers_of_mass(struct quadtree * qt)
{
	if (qt->state & _BH86_QTSTATE_EMPTY) { return; }
	else if (qt->state & _BH86_QTSTATE_FULL) { vec2_copy(&qt->center_of_mass, &qt->rb->pos); }
	else /* qt->state & _BH86_QTSTATE_DIVIDED */
	{
		vec2_div(&qt->center_of_mass, qt->total_mass, &qt->center_of_mass);
		for (struct quadtree * i = qt->childs; i < qt->childs + 4; ++i)
			quadtree_compute_centers_of_mass(i);
	}
}


static inline struct quadtree * quadtree_make_root(struct vec2 * pos, float_t max_axis_dist)
{
	struct quadtree * qt = malloc(sizeof(struct quadtree));
	if (qt == NULL) return NULL;

	float_t size = 1;
	// if (max_axis_dist <= 0.25)
	while (size >= 4 * max_axis_dist) size /= 2;
	// if (max_axis_dist > 0.5)
	while (size < 2 * max_axis_dist) size *= 2;

	memset(qt, 0, sizeof(struct quadtree));
	vec2_copy(&qt->pos, pos);
	qt->size = size;
	qt->state = _BH86_QTSTATE_EMPTY;

	return qt;
}


struct quadtree * quadtree_build(struct rbs_array * bodies)
{
	struct vec2 pos = {0, 0}, max_dist = {0, 0};
	struct rigidbody * rb;
	float_t d;
	for (struct rb_shape_base ** rbs_ptr = bodies->shapes; rbs_ptr < bodies->shapes + bodies->size; ++rbs_ptr)
	{
		rb = (struct rigidbody *)(*rbs_ptr);

		d = rb->pos.x;
		pos.x += d;
		d = fabs(d);
		if (d > max_dist.x) max_dist.x = d;

		d = rb->pos.y;
		pos.y += d;
		d = fabs(d);
		if (d > max_dist.y) max_dist.y = d;
	}
	vec2_div(&pos, bodies->size, &pos);
	vec2_sub(&max_dist, &pos, &max_dist);
	float_t max_axis_dist = max_dist.x > max_dist.y ? max_dist.x : max_dist.y;

	struct quadtree * qt = quadtree_make_root(&pos, max_axis_dist);
	if (qt == NULL) return NULL;

	for (struct rb_shape_base ** rbs_ptr = bodies->shapes; rbs_ptr < bodies->shapes + bodies->size; ++rbs_ptr)
	{
		if (quadtree_insert(qt, (struct rigidbody *)(*rbs_ptr)))
		{
			quadtree_delete(qt);
			return NULL;
		}
	}

	quadtree_compute_centers_of_mass(qt);
	return qt;
}


void _quadtree_delete(struct quadtree * qt)
{
	if (qt == NULL) return;

	if (qt->state & _BH86_QTSTATE_DIVIDED)
	{
		for (int i = 0; i < 4; ++i) _quadtree_delete(qt->childs + i);
		free(qt->childs);
	}
}

void quadtree_delete(struct quadtree * qt)
{
	_quadtree_delete(qt);
	free(qt);
}
