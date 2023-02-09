#include <errno.h>
#include <libtarpe2d.h>
#include <libtarpe2d_draw.h>


#define SHAPES_COUNT 100001
#define WIDTH 1280
#define HEIGHT 720
#define NO_VSYNC

#ifdef NO_VSYNC
#	define MAX_FPS 60
#	define MIN_DT (1. / MAX_FPS)
#endif


#define shape_check_null(shape_ptr, ret, cleanup_label)                    \
	do {                                                               \
		if (shape_ptr == NULL)                                     \
		{                                                          \
			printf("Couldn't allocate memory for a shape!\n"); \
			ret = ENOMEM;                                      \
			goto cleanup_label;                                \
		}                                                          \
	} while (0);


int main(void)
{
	int ret = 0;

	struct shape * shapes = malloc(sizeof(struct shape) * SHAPES_COUNT);
	if (shapes == NULL)
	{
		printf("Couldn't allocate memory for shapes array!\n");
		return ENOMEM;
	}

	// clang-format off
	// circ
	shapes[0].color = (SDL_Color){0xff, 0, 0, 0xff};
	shapes[0].rb_shape = (struct rb_shape_base *)rb_circle_new(
		14, 20,
		&(struct vec2){841, 650}, &(struct vec2){0},
		0, 0
	);
	shape_check_null(shapes[0].rb_shape, ret, cleanup_shapes);

	// rect
	shapes[1].color = (SDL_Color){0, 0xff, 0, 0xff};
	shapes[1].rb_shape = (struct rb_shape_base *)rb_rectangle_new(
		50, 100, 44,
		&(struct vec2){-WIDTH * 8.5, -HEIGHT * 5}, &(struct vec2){1000, 0},
		0.5, 2
	);
	shape_check_null(shapes[1].rb_shape, ret, cleanup_shapes);

	for (size_t i = 2; i < SHAPES_COUNT; ++i)
	{
		shapes[i].color = (SDL_Color){0, 0xff, 0, 0xff};
		shapes[i].rb_shape = (struct rb_shape_base *)rb_rectangle_copy((struct rb_rectangle *)shapes[1].rb_shape);
		shapes[i].rb_shape->rb.pos.x += i;
		shapes[i].rb_shape->rb.pos.y += (size_t)(i / 2);
		shape_check_null(shapes[i].rb_shape, ret, cleanup_shapes);
	}

	struct rb_ptr_array_iter iter = (struct rb_ptr_array_iter){
		.start = (struct rb_shape_base **)((int8_t *)shapes + offsetof(struct shape, rb_shape)),
		.step = sizeof(struct shape),
		.end = (struct rb_shape_base **)(shapes + SHAPES_COUNT)
	};

	// clang-format on

	GPU_Target * screen = tarpe2d_draw_init_flag(WIDTH,
						     HEIGHT,
						     true,
#ifdef NO_VSYNC
						     GPU_INIT_DISABLE_VSYNC
#else
						     GPU_DEFAULT_INIT_FLAGS
#endif
	);
	tarpe2d_draw_zoom_camera(screen, 1.4, 0.4);
	if (screen == NULL) goto cleanup_shapes;

	int is_done = 0;
	uint64_t start_time;
	double_t dt;
	char fps_title[8] = {0};
	start_time = SDL_GetPerformanceCounter();
	tarpe2d_draw(screen, shapes, SHAPES_COUNT);
	while (!is_done)
	{
#ifdef NO_VSYNC
		tarpe2d_draw_wait_until_min_dt(MIN_DT, start_time);
#endif
		dt = tarpe2d_draw_get_dt(&start_time);
		snprintf(fps_title, 7, "%6.2f", 1. / dt);
		SDL_SetWindowTitle(SDL_GetWindowFromID(screen->context->windowID), fps_title);

		is_done = tarpe2d_draw_poll_events();

		tarpe_tick_ptr_arr_iter(&iter, dt);

		tarpe2d_draw(screen, shapes, SHAPES_COUNT);
	}

	GPU_Quit();

cleanup_shapes:
	if (shapes != NULL)
	{
		for (size_t i = 0; i < SHAPES_COUNT && shapes[i].rb_shape != NULL; ++i)
			rb_shape_delete(shapes[i].rb_shape);
	}
	free(shapes);

	return ret;
}
