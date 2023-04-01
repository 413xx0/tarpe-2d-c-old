#include <libtarpe2d/bh86/bh.h>
#include <libtarpe2d/bh86/quadtree.h>
#include <libtarpe2d/config.h>
#include <stdint.h>


static inline void body_apply_node_force(struct rigidbody * rb, struct vec2 * dist,
					 float_t dist_magnitude, float_t other_mass)
{
	struct vec2 force;
	vec2_mul(dist,
		 (_LIBTARPE2D_CONFIG.grav_const * rb->mass * other_mass)
			 / (dist_magnitude * dist_magnitude * dist_magnitude),
		 &force);

	vec2_add(&rb->tick_force, &force, &rb->tick_force);
}

static inline float_t get_node_dist(struct rigidbody * rb, struct quadtree * node, struct vec2 * dist)
{
	vec2_sub(&node->center_of_mass, &rb->pos, dist);
	return vec2_abs(dist);
}

void body_apply_gravity_forces(struct rigidbody * rb, struct quadtree * qt)
{
	struct vec2 dist;
	float_t dist_magnitude;
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


int bh86_apply_gravity_forces(struct rbs_array * bodies)
{
	struct quadtree * qt = quadtree_build(bodies);
	if (qt == NULL) return 1;
	if (qt->state == _BH86_QTSTATE_EMPTY) return 0;

	for (struct rb_shape_base ** rbs_ptr = bodies->shapes; rbs_ptr < bodies->shapes + bodies->size; ++rbs_ptr)
	{
		body_apply_gravity_forces((struct rigidbody *)(*rbs_ptr), qt);
	}

	quadtree_delete(qt);
	return 0;
}
