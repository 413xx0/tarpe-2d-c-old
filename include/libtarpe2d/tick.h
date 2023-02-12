#ifndef __TICK_LIBTARPE2D_H__
#define __TICK_LIBTARPE2D_H__


#include <libtarpe2d/rb_shape_iter.h>


int tarpe_tick_ptr_arr_iter(struct rb_ptr_array_iter * iter, double_t dt);

int tarpe_tick_uni_iter(struct rb_uni_iter * iter, double_t dt);


#endif /*__TICK_LIBTARPE2D_H__*/
