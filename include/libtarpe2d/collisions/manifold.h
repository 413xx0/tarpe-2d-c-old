#ifndef __LIBTARPE2D_COLLISIONS_MANIFOLD_H__
#define __LIBTARPE2D_COLLISIONS_MANIFOLD_H__

#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/vec2.h>
#include <tgmath.h>


struct manifold
{
	struct rb_shape_base *body_1, *body_2;

	struct vec2 normal;
	float_t depth;

	struct vec2 contact_points[2];
	int contact_points_count;
};


#endif /*__LIBTARPE2D_COLLISIONS_MANIFOLD_H__*/
