#ifndef __LIBTARPE2D_RB_SHAPE_ARRAY_H__
#define __LIBTARPE2D_RB_SHAPE_ARRAY_H__

#include <libtarpe2d/rb_shape.h>
#include <stddef.h>


struct rbs_array
{
	struct rb_shape_base ** shapes;
	size_t size;
};


#endif /*__LIBTARPE2D_RB_SHAPE_ARRAY_H__*/
