#ifndef __LIBTARPE2D_COLLISIONS_NARROW_PHASE_H__
#define __LIBTARPE2D_COLLISIONS_NARROW_PHASE_H__

#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/vec2.h>
#include <stdbool.h>


// static inline bool is_colliding_circle_circle(struct vec2 * pos_1, struct s_circle * circ_1,
// 					      struct vec2 * pos_2, struct s_circle * circ_2)
// {
// 	return vec2_sqr_dist(pos_2, pos_1) <= flt_squared(circ_1->radius + circ_2->radius);
// }

// bool is_colliding_circle_rect(struct vec2 * pos_circ, struct s_circle * circ, struct vec2 * pos_rect,
// 			      struct s_rectangle * rect);

// bool is_colliding_rect_rect(struct vec2 * pos_1, struct s_rectangle * rect_1, struct vec2 * pos_2,
// 			    struct s_rectangle * rect_2);

bool get_penetration(struct rb_shape_base * body_1, struct rb_shape_base * body_2,
		     struct vec2 * penetration_out);


#endif /*__LIBTARPE2D_COLLISIONS_NARROW_PHASE_H__*/
