#ifndef __LIBTARPE2D_COLLISIONS_SOLVE_COLLISION_H__
#define __LIBTARPE2D_COLLISIONS_SOLVE_COLLISION_H__

#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/vec2.h>


void solve_collision(struct rb_shape_base * body_1, struct rb_shape_base * body_2,
		     struct vec2 * penetration);


#endif /*__LIBTARPE2D_COLLISIONS_SOLVE_COLLISION_H__*/
