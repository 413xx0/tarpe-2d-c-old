#ifndef __LIBTARPE2D_COLLISIONS_NARROW_PHASE_H__
#define __LIBTARPE2D_COLLISIONS_NARROW_PHASE_H__

#include <libtarpe2d/collisions/manifold.h>
#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/vec2.h>
#include <stdbool.h>


bool get_penetration(struct manifold * manifold);


#endif /*__LIBTARPE2D_COLLISIONS_NARROW_PHASE_H__*/
