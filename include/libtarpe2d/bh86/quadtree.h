#ifndef __LIBTARPE2D_BH86_QUADTREE_H__
#define __LIBTARPE2D_BH86_QUADTREE_H__


#include <libtarpe2d/rb_shape_iter.h>
#include <libtarpe2d/rigidbody.h>
#include <libtarpe2d/vec2.h>
#include <stddef.h>
#include <tgmath.h>


enum quadtree_node_state
{
	_BH86_QTSTATE_EMPTY = 1 << 0,
	_BH86_QTSTATE_FULL = 1 << 1,
	_BH86_QTSTATE_DIVIDED = 1 << 2,
};


struct quadtree
{
	struct vec2 pos;
	float_t size;
	enum quadtree_node_state state;

	float_t total_mass;
	struct vec2 center_of_mass; // NOT valid when quadtree is building
	struct rigidbody * rb; // valid only for state == _BH86_QTSTATE_FULL

	struct quadtree * childs;
};


struct quadtree * quadtree_build(struct rbs_iter * bodies);


void quadtree_delete(struct quadtree * qt);


#endif /*__LIBTARPE2D_BH86_QUADTREE_H__*/
