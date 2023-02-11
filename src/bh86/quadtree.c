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

	double_t half_size = qt->size / 2;
	double_t dp_x = -half_size / 2, dp_y = dp_x;
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

static inline void quadtree_empty_insert(struct quadtree * qt, struct vec2 * rb_pos, double_t rb_mass)
{
	qt->rb_pos = rb_pos;
	vec2_mul(rb_pos, rb_mass, &qt->center_of_mass);
	qt->total_mass = rb_mass;
	qt->state = _BH86_QTSTATE_FULL;
}

int quadtree_insert(struct quadtree * qt, struct rigidbody * rb)
{
	if (qt->state & _BH86_QTSTATE_EMPTY) { quadtree_empty_insert(qt, &rb->pos, rb->mass); }
	else
	{
		if (qt->state & _BH86_QTSTATE_FULL)
		{
			if (quadtree_divide(qt)) return 1;
			quadtree_empty_insert(qt->childs + quadtree_quarter(qt, qt->rb_pos),
					      qt->rb_pos,
					      qt->total_mass);
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
	else if (qt->state & _BH86_QTSTATE_FULL) { vec2_copy(&qt->center_of_mass, qt->rb_pos); }
	else /* qt->state & _BH86_QTSTATE_DIVIDED */
	{
		vec2_div(&qt->center_of_mass, qt->total_mass, &qt->center_of_mass);
		for (struct quadtree * i = qt->childs; i < qt->childs + 4; ++i)
			quadtree_compute_centers_of_mass(i);
	}
}


static inline struct quadtree * quadtree_make_root(double_t max_axis_dist)
{
	struct quadtree * qt = malloc(sizeof(struct quadtree));
	if (qt == NULL) return NULL;

	double_t size = 1;
	// if (max_axis_dist <= 0.25)
	while (size >= 4 * max_axis_dist) size /= 2;
	// if (max_axis_dist > 0.5)
	while (size < 2 * max_axis_dist) size *= 2;

	memset(qt, 0, sizeof(struct quadtree));
	qt->size = size;
	qt->state = _BH86_QTSTATE_EMPTY;

	return qt;
}


struct quadtree * quadtree_build_ptr_arr_iter(struct rb_ptr_array_iter * bodies)
{
	double_t max_axis_dist = 0., d;
	for (struct rb_shape_base ** i = bodies->start; i < bodies->end;
	     i = (struct rb_shape_base **)((int8_t *)i + bodies->step))
	{
		d = fabs(((struct rigidbody *)*i)->pos.x);
		if (d > max_axis_dist) max_axis_dist = d;

		d = fabs(((struct rigidbody *)*i)->pos.y);
		if (d > max_axis_dist) max_axis_dist = d;
	}

	struct quadtree * qt = quadtree_make_root(max_axis_dist);
	if (qt == NULL) return NULL;

	for (struct rb_shape_base ** i = bodies->start; i < bodies->end;
	     i = (struct rb_shape_base **)((int8_t *)i + bodies->step))
	{
		if (quadtree_insert(qt, ((struct rigidbody *)*i)))
		{
			quadtree_delete(qt);
			return NULL;
		}
	}
	quadtree_compute_centers_of_mass(qt);
	return qt;
}

struct quadtree * quadtree_build_uni_iter(struct rb_uni_iter * bodies)
{
	void * userdata = malloc(bodies->userdata_size);
	if (userdata == NULL) return NULL;

	double_t max_axis_dist = 0., d;
	struct rb_shape_base * i = bodies->get_first(bodies->data_structure, userdata);
	while (i != NULL)
	{
		d = fabs(((struct rigidbody *)i)->pos.x);
		if (d > max_axis_dist) max_axis_dist = d;

		d = fabs(((struct rigidbody *)i)->pos.y);
		if (d > max_axis_dist) max_axis_dist = d;

		i = bodies->get_next(userdata);
	}

	struct quadtree * qt = quadtree_make_root(max_axis_dist);
	if (qt == NULL) return NULL;

	memset(userdata, 0, bodies->userdata_size);
	i = bodies->get_first(bodies->data_structure, userdata);
	while (i != NULL)
	{
		if (quadtree_insert(qt, *((struct rigidbody **)i)))
		{
			quadtree_delete(qt);
			return NULL;
		}
		i = bodies->get_next(userdata);
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
