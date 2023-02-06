#ifndef __LIBTARPE2d_VEC2_H__
#define __LIBTARPE2d_VEC2_H__


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>


struct vec2
{
	float_t x;
	float_t y;
};

static inline void vec2_set(struct vec2 * v, float_t x, float_t y)
{
	v->x = x;
	v->y = y;
}

static inline bool vec2_cmp(struct vec2 * v, struct vec2 * u) { return v->x == u->x && v->y == u->y; }

static inline void vec2_copy(struct vec2 * dest, struct vec2 * src)
{
	memcpy(dest, src, sizeof(struct vec2));
}

static inline void vec2_neg(struct vec2 * v, struct vec2 * out)
{
	out->x = -v->x;
	out->y = -v->y;
}

static inline float_t vec2_sqr_abs(struct vec2 * v) { return v->x * v->x + v->y * v->y; }

static inline float_t vec2_abs(struct vec2 * v) { return sqrt(v->x * v->x + v->y * v->y); }

static inline void vec2_add(struct vec2 * v, struct vec2 * u, struct vec2 * out)
{
	out->x = v->x + u->x;
	out->y = v->y + u->y;
}

static inline void vec2_sub(struct vec2 * left, struct vec2 * right, struct vec2 * out)
{
	out->x = left->x - right->x;
	out->y = left->y - right->y;
}

static inline void vec2_mul(struct vec2 * v, float_t u, struct vec2 * out)
{
	out->x = v->x * u;
	out->y = v->y * u;
}

static inline void vec2_div(struct vec2 * left, float_t right, struct vec2 * out)
{
	out->x = left->x / right;
	out->y = left->y / right;
}

static inline bool vec2_is_zero(struct vec2 * v) { return v->x == 0 && v->y == 0; }

static inline float_t vec2_dot(struct vec2 * v, struct vec2 * u) { return v->x * u->x + v->y * u->y; }

static inline float_t vec2_cross_abs(struct vec2 * left, struct vec2 * right)
{
	return left->x * right->y - left->y * right->x;
}

static inline void vec2_norm(struct vec2 * v, struct vec2 * out)
{
	if (vec2_is_zero(v))
		vec2_set(out, 0, 0);
	else
		vec2_div(v, vec2_abs(v), out);
}

static inline float_t vec2_angle(struct vec2 * v, struct vec2 * u)
{
	return acos(vec2_dot(v, u) / (vec2_abs(v) * vec2_abs(u)));
}

static inline float_t vec2_sqr_dist(struct vec2 * v, struct vec2 * u)
{
	struct vec2 tmp;
	vec2_sub(v, u, &tmp);
	return vec2_sqr_abs(&tmp);
}

static inline float_t vec2_dist(struct vec2 * v, struct vec2 * u)
{
	struct vec2 tmp;
	vec2_sub(v, u, &tmp);
	return vec2_abs(&tmp);
}


static inline void vec2_print(struct vec2 * v) { printf("{x: %lf; y: %lf}\n", v->x, v->y); }


#endif /*__LIBTARPE2d_VEC2_H__*/
