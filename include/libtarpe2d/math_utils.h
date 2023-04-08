#ifndef __LIBTARPE2D_MATH_UTILS_H__
#define __LIBTARPE2D_MATH_UTILS_H__

#include <float.h>
#include <tgmath.h>


#if FLT_EVAL_METHOD == 0
#	define FLT_T_MAX FLT_MAX
#elif FLT_EVAL_METHOD == 1
#	define FLT_T_MAX DBL_MAX
#elif FLT_EVAL_METHOD == 2
#	define FLT_T_MAX LDBL_MAX
#else
#	error FLT_EVAL_METHOD has unknown value
#endif


static inline float_t flt_squared(float_t num) { return num * num; }


#endif /*__LIBTARPE2D_MATH_UTILS_H__*/
