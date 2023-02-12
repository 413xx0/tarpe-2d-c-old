#ifndef __LIBTARPE2d_RB_ITER_H__
#define __LIBTARPE2d_RB_ITER_H__


#include <libtarpe2d/rb_shape.h>
#include <stddef.h>


struct rb_ptr_array_iter
{
	struct rb_shape_base ** start;
	size_t step;
	struct rb_shape_base ** end;
	size_t rb_count;
};


struct rb_uni_iter
{
	void * data_structure;
	size_t userdata_size;
	struct rb_shape_base * (*get_first)(void * data_structure, void * userdata);
	struct rb_shape_base * (*get_next)(void * userdata);
	size_t rb_count;
};


#endif /*__LIBTARPE2d_RB_ITER_H__*/
