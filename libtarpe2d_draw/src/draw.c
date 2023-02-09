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
		if (ev.type == SDL_QUIT)
			return 1;
	}
	return 0;
}


#define vec2_to_arr(arr_name, idx_x, idx_y, vec2) \
	arr_name[idx_x] = vec2.x;                 \
	arr_name[idx_y] = vec2.y;

void tarpe2d_draw(GPU_Target * screen, struct shape * shapes_arr, size_t shapes_count)
{
	GPU_Clear(screen);

	float vertices[8];
	struct vec2 rot_half_width, rot_half_height;
	struct vec2 left, right, tmp, pos;
	double_t sin_rot, cos_rot, radius;
	bool flag;
	double_t w_min = 0, w_max = screen->w, h_min = 0, h_max = screen->h;
	if (GPU_IsCameraEnabled(screen))
	{
		double_t dw = screen->w * (1 / screen->camera.zoom_x - 1) / 2,
			 dh = screen->h * (1 / screen->camera.zoom_y - 1) / 2;
		w_min -= dw;
		w_max += dw;
		h_min -= dh;
		h_max += dh;
	}
	for (struct shape * i = shapes_arr; i < shapes_arr + shapes_count; ++i)
	{
		if (i->rb_shape->type == TARPE__RB_SHAPE_TYPE__CIRCLE)
		{
			radius = ((struct rb_circle *)(i->rb_shape))->radius;
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
			sin_rot = sin(i->rb_shape->rb.angle);
			cos_rot = cos(i->rb_shape->rb.angle);
			rot_half_width = (struct vec2){
				.x = ((struct rb_rectangle *)(i->rb_shape))->half_side_sizes.x * cos_rot,
				.y = ((struct rb_rectangle *)(i->rb_shape))->half_side_sizes.x
				     * sin_rot};
			rot_half_height = (struct vec2){
				.x = ((struct rb_rectangle *)(i->rb_shape))->half_side_sizes.y
				     * -sin_rot,
				.y = ((struct rb_rectangle *)(i->rb_shape))->half_side_sizes.y
				     * cos_rot};

			vec2_add(&i->rb_shape->rb.pos, &rot_half_width, &right);
			vec2_sub(&i->rb_shape->rb.pos, &rot_half_width, &left);

			// vert 1
			vec2_add(&left, &rot_half_height, &tmp);
			vec2_to_arr(vertices, 0, 1, tmp);

			// vert 2
			vec2_add(&right, &rot_half_height, &tmp);
			vec2_to_arr(vertices, 2, 3, tmp);

			// vert 3
			vec2_sub(&right, &rot_half_height, &tmp);
			vec2_to_arr(vertices, 4, 5, tmp);

			// vert 4
			vec2_sub(&left, &rot_half_height, &tmp);
			vec2_to_arr(vertices, 6, 7, tmp);

			flag = false;
			for (int i = 0; i < 8; i += 2)
			{
				pos.x = vertices[i];
				pos.y = vertices[i + 1];
				if ((pos.x >= w_min && pos.x <= w_max)
				    && (pos.y >= h_min && pos.y <= h_max))
				{
					flag = true;
					break;
				}
			}
			for (int i = 1; i < 8; i += 2) vertices[i] = screen->h - vertices[i];

			if (flag) GPU_PolygonFilled(screen, 4, vertices, i->color);
		}
	}

	GPU_Flip(screen);
}
