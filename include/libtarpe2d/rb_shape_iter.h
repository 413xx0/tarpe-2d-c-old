#ifndef __LIBTARPE2d_RB_SHAPE_ITER_H__
#define __LIBTARPE2d_RB_SHAPE_ITER_H__


#include <libtarpe2d/rb_shape.h>
#include <stddef.h>


typedef unsigned int (*rbs_iter_step_func_pt)(struct rb_shape_base * rb_shape, void * userdata);
typedef int (*rbs_iter_func_pt)(void * data_structure, rbs_iter_step_func_pt step_func,
				void * step_func_data);


struct rbs_iter
{
	void * data_structure;
	rbs_iter_func_pt iter_func;
	size_t size;
};


#endif /*__LIBTARPE2d_RB_SHAPE_ITER_H__*/
