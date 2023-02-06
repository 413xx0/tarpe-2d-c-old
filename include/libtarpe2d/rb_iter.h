#ifndef __LIBTARPE2d_RB_ITER_H__
#define __LIBTARPE2d_RB_ITER_H__


#include <libtarpe2d/rigidbody.h>
#include <stddef.h>


struct rb_array_iter
{
	struct rigidbody * start;
	size_t step;
	struct rigidbody * end;
};


struct rb_uni_iter
{
	void * data_structure;
	size_t userdata_size;
	struct rigidbody * (*get_first)(void * data_structure, void * userdata);
	struct rigidbody * (*get_next)(void * userdata);
};


#endif /*__LIBTARPE2d_RB_ITER_H__*/
