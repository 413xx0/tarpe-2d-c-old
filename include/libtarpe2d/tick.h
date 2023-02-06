#ifndef __TICK_LIBTARPE2D_H__
#define __TICK_LIBTARPE2D_H__


#include <libtarpe2d/rb_iter.h>


void tarpe_tick_arr_iter(struct rb_array_iter * iter, float_t dt);

int tarpe_tick_uni_iter(struct rb_uni_iter * iter, float_t dt);


#endif /*__TICK_LIBTARPE2D_H__*/
