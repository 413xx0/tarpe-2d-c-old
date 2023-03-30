#ifndef __LIBTARPE2D_SHAPE_H__
#define __LIBTARPE2D_SHAPE_H__


#include <libtarpe2d/vec2.h>
#include <tgmath.h>


enum tarpe_shape_type
{
	TARPE__SHAPE_TYPE__CIRCLE,
	TARPE__SHAPE_TYPE__RECTANGLE
};


struct s_circle
{
	float_t radius;
};


struct s_rectangle
{
	// struct vec2 side_sizes;
	struct vec2 half_side_sizes;
};


#endif /*__LIBTARPE2D_SHAPE_H__*/
