#ifndef __LIBTARPE2D_DRAW_DRAW_H__
#define __LIBTARPE2D_DRAW_DRAW_H__


#include <SDL2/SDL_gpu.h>
#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d_draw/shape.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


GPU_Target *
tarpe2d_draw_init_flag(int width, int height, bool is_camera_enabled, GPU_WindowFlagEnum flags);

static inline GPU_Target * tarpe2d_draw_init(int width, int height, bool is_camera_enabled)
{
	return tarpe2d_draw_init_flag(width, height, is_camera_enabled, GPU_DEFAULT_INIT_FLAGS);
}


int tarpe2d_draw_zoom_camera(GPU_Target * screen, float zoom_x, float zoom_y);

static inline int tarpe2d_draw_zoom_camera_xy(GPU_Target * screen, float zoom)
{
	return tarpe2d_draw_zoom_camera(screen, zoom, zoom);
}


int tarpe2d_draw_poll_events(void);

void tarpe2d_draw(GPU_Target * screen, struct shape * shapes_arr, size_t shapes_count, bool do_flip);


static inline double_t tarpe2d_draw_get_dt(uint64_t * frame_start)
{
	uint64_t end_time = SDL_GetPerformanceCounter();
	double_t dt = (end_time - *frame_start) / (double_t)SDL_GetPerformanceFrequency();
	*frame_start = end_time;
	return dt;
}

static inline void tarpe2d_draw_wait_until_min_dt(double_t min_dt, uint64_t frame_start)
{
	while ((SDL_GetPerformanceCounter() - frame_start) / (double_t)SDL_GetPerformanceFrequency()
	       < min_dt)
		;
}


#endif /*__LIBTARPE2D_DRAW_DRAW_H__*/
