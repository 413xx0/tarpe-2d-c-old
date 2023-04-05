#include <SDL2/SDL.h>
#include <SDL2/SDL_gpu.h>
#include <libtarpe2d/rb_shape.h>
#include <libtarpe2d/vec2.h>
#include <libtarpe2d_draw/draw.h>
#include <libtarpe2d_draw/shape.h>
#include <tgmath.h>


GPU_Target *
tarpe2d_draw_init_flag(int width, int height, bool is_camera_enabled, GPU_WindowFlagEnum flags)
{
	GPU_Target * screen = GPU_Init(width, height, flags);

	if (screen == NULL)
	{
		GPU_LogError("Failed to init SDL_gpu screen!\n");

		GPU_ErrorObject err;
		while (err = GPU_PopErrorCode(), err.error != GPU_ERROR_NONE)
		{
			GPU_LogError("(%s): %s\n", err.function, err.details);
		}

		return NULL;
	}

	if (flags & GPU_INIT_DISABLE_VSYNC && screen->renderer->id.renderer >= GPU_RENDERER_OPENGL_1_BASE
	    && screen->renderer->id.renderer <= GPU_RENDERER_OPENGL_4)
		SDL_GL_SetSwapInterval(0);

	GPU_EnableCamera(screen, is_camera_enabled);
	GPU_SetCoordinateMode(true);

	return screen;
}


int tarpe2d_draw_zoom_camera(GPU_Target * screen, float zoom_x, float zoom_y)
{
	if (!GPU_IsCameraEnabled(screen)) return -1;

	GPU_Camera camera = GPU_GetCamera(screen);
	camera.zoom_x = zoom_x;
	camera.zoom_y = zoom_y;
	GPU_SetCamera(screen, &camera);

	return 0;
}


int tarpe2d_draw_poll_events(void)
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		if (ev.type == SDL_QUIT) return 1;
	}
	return 0;
}


#define vec2_to_arr(arr_name, idx_x, idx_y, vec2) \
	arr_name[idx_x] = vec2.x;                 \
	arr_name[idx_y] = vec2.y;

void tarpe2d_draw(GPU_Target * screen, struct shape * shapes_arr, size_t shapes_count, bool do_flip)
{
	GPU_Clear(screen);

	float vertices[8];
	struct vec2 vec2_vertices[4];
	struct vec2 pos;
	float_t radius;
	bool flag;
	float_t w_min = 0, w_max = screen->w, h_min = 0, h_max = screen->h;
	if (GPU_IsCameraEnabled(screen))
	{
		float_t dw = screen->w * (1 / screen->camera.zoom_x - 1) / 2,
			dh = screen->h * (1 / screen->camera.zoom_y - 1) / 2;
		w_min -= dw;
		w_max += dw;
		h_min -= dh;
		h_max += dh;
	}
	for (struct shape * i = shapes_arr; i < shapes_arr + shapes_count; ++i)
	{
		if (i->rb_shape->type == TARPE__SHAPE_TYPE__CIRCLE)
		{
			radius = ((struct rb_circle *)(i->rb_shape))->circle.radius;
			pos.x = i->rb_shape->rb.pos.x;
			pos.y = i->rb_shape->rb.pos.y;
			if ((pos.x + radius >= w_min && pos.x - radius <= w_max)
			    && (pos.y + radius >= h_min && pos.y - radius <= h_max))
				GPU_CircleFilled(screen,
						 i->rb_shape->rb.pos.x,
						 screen->h - i->rb_shape->rb.pos.y,
						 radius,
						 i->color);
		}
		else
		{
			rb_rectangle_get_vertices((struct rb_rectangle *)i->rb_shape, vec2_vertices);

			flag = false;
			for (int i = 0; i < 4; ++i)
			{
				pos = vec2_vertices[i];
				vertices[i * 2] = pos.x;
				vertices[i * 2 + 1] = screen->h - pos.y;
				if (!flag
				    || ((pos.x >= w_min && pos.x <= w_max)
					&& (pos.y >= h_min && pos.y <= h_max)))
					flag = true;
			}

			if (flag) GPU_PolygonFilled(screen, 4, vertices, i->color);
		}
	}

	if (do_flip) GPU_Flip(screen);
}
