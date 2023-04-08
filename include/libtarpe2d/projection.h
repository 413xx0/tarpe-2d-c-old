#ifndef __LIBTARPE2D_PROJECTION_H__
#define __LIBTARPE2D_PROJECTION_H__

#include <stdbool.h>
#include <tgmath.h>


struct projection
{
	float_t min, max;
};

static inline float_t projection_get_overlap(struct projection * proj_1, struct projection * proj_2)
{
	if (proj_1->min < proj_2->max && proj_2->min < proj_1->max)
		return fmin(proj_1->max, proj_2->max) - fmax(proj_1->min, proj_2->min);
	return 0;
}

static inline bool
projection_is_contained_exclusive(struct projection * outer, struct projection * inner)
{
	return inner->min > outer->min && inner->max < outer->max;
}


#endif /*__LIBTARPE2D_PROJECTION_H__*/
