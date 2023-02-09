#ifndef __LIBTARPE2D_DRAW_SHAPE_H__
#define __LIBTARPE2D_DRAW_SHAPE_H__


#include <SDL2/SDL.h>
#include <libtarpe2d/rb_shape.h>


struct shape
{
	struct rb_shape_base * rb_shape;
	SDL_Color color;
};


#endif /*__LIBTARPE2D_DRAW_SHAPE_H__*/
