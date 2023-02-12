#include <libtarpe2d/bh86/bh.h>
#include <libtarpe2d/config.h>
#include <stdint.h>


static inline void body_apply_node_force(struct rigidbody * rb, struct vec2 * dist,
					 double_t dist_magnitude, double_t other_mass)
{
	struct vec2 force;
	vec2_mul(dist,
		 (_LIBTARPE2D_CONFIG.grav_const * rb->mass * other_mass)
			 / (dist_magnitude * dist_magnitude * dist_magnitude),
		 &force);

	vec2_add(&rb->tick_force, &force, &rb->tick_force);
}

static inline double_t get_node_dist(struct rigidbody * rb, struct quadtree * node, struct vec2 * dist)
{
	vec2_sub(&node->center_of_mass, &rb->pos, dist);
	return vec2_abs(dist);
}

void body_apply_gravity_forces(struct rigidbody * rb, struct quadtree * qt)
{
	struct vec2 dist;
	double_t dist_magnitude;
	dist_magnitude = get_node_dist(rb, qt, &dist);
	if (dist_magnitude == 0) return;

	if (qt->state & _BH86_QTSTATE_FULL)
	{
		body_apply_node_force(rb, &dist, dist_magnitude, qt->total_mass);
	}
	else
	{
		if (qt->size / dist_magnitude < _LIBTARPE2D_CONFIG.bh86_threshold)
		{
			body_apply_node_force(rb, &dist, dist_magnitude, qt->total_mass);
		}
		else
		{
			for (struct quadtree * child = qt->childs; child < qt->childs + 4; ++child)
			{
				if (child->state & _BH86_QTSTATE_EMPTY) continue;
				body_apply_gravity_forces(rb, child);
			}
		}
	}
}


int bh86_apply_gravity_forces_ptr_arr_iter(struct rb_ptr_array_iter * bodies)
{
	struct quadtree * qt;
	qt = quadtree_build_ptr_arr_iter(bodies);
	if (qt == NULL) return 1;
	if (qt->state == _BH86_QTSTATE_EMPTY) return 0;

	for (struct rb_shape_base ** i = bodies->start; i < bodies->end;
	     i = (struct rb_shape_base **)((int8_t *)i + bodies->step))
	{
		body_apply_gravity_forces((struct rigidbody *)*i, qt);
	}

	quadtree_delete(qt);
	// struct rigidbody *a, *b;
	// struct vec2 dist;
	// double_t dist_magnitude;
	// for (struct rb_shape_base ** i = bodies->start; i < bodies->end;
	//      i = (struct rb_shape_base **)((int8_t *)i + bodies->step))
	// {
	// 	for (struct rb_shape_base ** j = (struct rb_shape_base **)((int8_t *)i + bodies->step);
	// 	     j < bodies->end;
	// 	     j = (struct rb_shape_base **)((int8_t *)j + bodies->step))
	// 	{
	// 		a = (struct rigidbody *)*i;
	// 		b = (struct rigidbody *)*j;
	// 		vec2_sub(&b->pos, &a->pos, &dist);
	// 		dist_magnitude = vec2_abs(&dist);
	// 		body_apply_node_force(a, &dist, dist_magnitude, b->mass);
	// 		vec2_neg(&dist, &dist);
	// 		body_apply_node_force(b, &dist, dist_magnitude, a->mass);
	// 	}
	// }
	return 0;
}

int bh86_apply_gravity_forces_uni_iter(struct rb_uni_iter * bodies)
{
	struct quadtree * qt;
	qt = quadtree_build_uni_iter(bodies);
	if (qt == NULL) return 1;
	if (qt->state == _BH86_QTSTATE_EMPTY) return 0;

	void * userdata = malloc(bodies->userdata_size);
	if (userdata == NULL)
	{
		quadtree_delete(qt);
		return 1;
	}

	struct rb_shape_base * i = bodies->get_first(bodies->data_structure, userdata);
	while (i != NULL)
	{
		body_apply_gravity_forces((struct rigidbody *)i, qt);

		i = bodies->get_next(userdata);
	}

	quadtree_delete(qt);
	return 0;
}
