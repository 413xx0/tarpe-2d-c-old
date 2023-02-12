#ifndef __LIBTARPE2D_BH86_BH_H__
#define __LIBTARPE2D_BH86_BH_H__


#include <libtarpe2d/bh86/quadtree.h>


int bh86_apply_gravity_forces_ptr_arr_iter(struct rb_ptr_array_iter * bodies);

int bh86_apply_gravity_forces_uni_iter(struct rb_uni_iter * bodies);


#endif /*__LIBTARPE2D_BH86_BH_H__*/
