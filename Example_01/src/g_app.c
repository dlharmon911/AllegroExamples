#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

typedef struct g_point_t
{
	float m_width;
	float m_height;
} g_point_t;

typedef struct g_app_data_t
{
	uint8_t m_key[ALLEGRO_KEY_MAX];
	ALLEGRO_DISPLAY* m_display;
	ALLEGRO_TIMER* m_logic_timer;
	ALLEGRO_EVENT_QUEUE* m_event_queue;
	ALLEGRO_BITMAP* m_inventory;
	ALLEGRO_BITMAP* m_double_buffer;
	ALLEGRO_FONT* m_font;
	int32_t m_mode;
	double m_fps;
	bool m_is_running;
	bool m_update_logic;
} g_app_data_t;

enum G_DRAWING_MODE
{
	G_MODE_DEFAULT,
	G_MODE_DOUBLE_BUFFER,
	G_MODE_TRANSFORM,
	G_MODE_COUNT
};

static const char* const G_TITLE = "Drawing Bitmaps!";
static const g_point_t G_DISPLAY_INITIAL_SIZE = { 500.0f, 500.0f };
static const g_point_t G_GAME_SCREEN_SIZE = { 500.0f, 500.0f };
static const double G_LOGIC_TIMER_RATE = 60.0;
static const ALLEGRO_COLOR G_COLOR_EIGENGRAU = { 0.08627451f, 0.08627451f, 0.11372549f, 1.0f };
static const ALLEGRO_COLOR G_COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
static const char* G_MODE_NAME[G_MODE_COUNT] =
{
	"Default",
	"Double Buffer",
	"Transform"
};

enum
{
	G_FPS_ARRAY_COUNT = 100
};


void g_app_zero_initialize_data(g_app_data_t* data);
int32_t g_app_initialize(g_app_data_t* data);
void g_app_shutdown(g_app_data_t* data);
void g_app_loop(g_app_data_t* data);

int32_t main(int32_t argc, char** argv)
{
	g_app_data_t data;

	g_app_zero_initialize_data(&data);

	if (g_app_initialize(&data) == 0)
	{
		g_app_loop(&data);
	}

	g_app_shutdown(&data);

	return 0;
}

static void g_app_zero_initialize_data(g_app_data_t* data)
{
	for (size_t i = 0; i < ALLEGRO_KEY_MAX; ++i)
	{
		data->m_key[i] = false;
	}

	data->m_display = NULL;
	data->m_logic_timer = NULL;
	data->m_event_queue = NULL;
	data->m_inventory = NULL;
	data->m_double_buffer = NULL;
	data->m_font = NULL;
	data->m_mode = G_MODE_DEFAULT;
	data->m_fps = 0.0;
	data->m_is_running = true;
	data->m_update_logic = false;
}

int32_t g_app_initialize(g_app_data_t* data)
{
	if (!al_init())
	{
		return -1;
	}

	if (!(al_init_image_addon()))
	{
		return -1;
	}

	if (!(al_init_font_addon()))
	{
		return -1;
	}

	if (!al_install_keyboard())
	{
		return -1;
	}

	if (!al_install_mouse())
	{
		return -1;
	}

	al_set_new_window_title(G_TITLE);
	al_set_new_display_flags(ALLEGRO_RESIZABLE | ALLEGRO_WINDOWED);
	if (!(data->m_display = al_create_display((int32_t)G_DISPLAY_INITIAL_SIZE.m_width, (int32_t)G_DISPLAY_INITIAL_SIZE.m_height)))
	{
		return -1;
	}

	if (!(data->m_double_buffer = al_create_bitmap((int32_t)G_GAME_SCREEN_SIZE.m_width, (int32_t)G_GAME_SCREEN_SIZE.m_height)))
	{
		return -1;
	}

	if (!(data->m_logic_timer = al_create_timer(1.0 / G_LOGIC_TIMER_RATE)))
	{
		return -1;
	}

	if (!(data->m_event_queue = al_create_event_queue()))
	{
		return -1;
	}

	if (!(data->m_inventory = al_load_bitmap("assets/inventory.png")))
	{
		return -1;
	}

	if (!(data->m_font = al_create_builtin_font()))
	{
		return -1;
	}

	al_register_event_source(data->m_event_queue, al_get_display_event_source(data->m_display));
	al_register_event_source(data->m_event_queue, al_get_timer_event_source(data->m_logic_timer));
	al_register_event_source(data->m_event_queue, al_get_keyboard_event_source());
	al_register_event_source(data->m_event_queue, al_get_mouse_event_source());

	return 0;
}

void g_app_shutdown(g_app_data_t* data)
{
	if (data->m_font)
	{
		al_destroy_font(data->m_font);
		data->m_font = NULL;
	}

	if (data->m_inventory)
	{
		al_destroy_bitmap(data->m_inventory);
		data->m_inventory = NULL;
	}

	if (data->m_event_queue)
	{
		al_destroy_event_queue(data->m_event_queue);
		data->m_event_queue = NULL;
	}

	if (data->m_logic_timer)
	{
		al_destroy_timer(data->m_logic_timer);
		data->m_logic_timer = NULL;
	}

	if (data->m_display)
	{
		al_destroy_display(data->m_display);
		data->m_display = NULL;
	}

	if (al_is_mouse_installed())
	{
		al_uninstall_mouse();
	}

	if (al_is_keyboard_installed())
	{
		al_uninstall_keyboard();
	}

	if (al_is_system_installed())
	{
		al_uninstall_system();
	}
}

static void g_app_input(g_app_data_t* data)
{
	static ALLEGRO_EVENT event;

	while (!al_is_event_queue_empty(data->m_event_queue))
	{
		al_get_next_event(data->m_event_queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
		{
			data->m_update_logic = true;
		} break;

		case ALLEGRO_EVENT_KEY_UP:
		{
			data->m_key[event.keyboard.keycode] = false;
		} break;

		case ALLEGRO_EVENT_KEY_DOWN:
		{
			data->m_key[event.keyboard.keycode] = true;
		} break;

		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			al_acknowledge_resize(event.display.source);
		} break;

		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			data->m_is_running = false;
		} break;
		default: return;
		}
	}
}

static void g_app_logic(g_app_data_t* data)
{
	if (data->m_key[ALLEGRO_KEY_ESCAPE])
	{
		data->m_is_running = false;
	}

	if (data->m_key[ALLEGRO_KEY_1])
	{
		data->m_mode = G_MODE_DEFAULT;
	}
	if (data->m_key[ALLEGRO_KEY_2])
	{
		data->m_mode = G_MODE_DOUBLE_BUFFER;
	}
	if (data->m_key[ALLEGRO_KEY_3])
	{
		data->m_mode = G_MODE_TRANSFORM;
	}
}

static void g_app_draw_inventory(const g_app_data_t* data)
{
	al_clear_to_color(G_COLOR_EIGENGRAU);
	al_draw_bitmap(data->m_inventory, 0.0f, 0.0f, 0);
}

static void g_app_draw_mode_double_buffer(const g_app_data_t* data)
{
	ALLEGRO_BITMAP* target = al_get_target_bitmap();
	al_set_target_bitmap(data->m_double_buffer);

	g_app_draw_inventory(data);

	al_set_target_bitmap(target);
	al_draw_scaled_bitmap(data->m_double_buffer,
		0.0f, 0.0f,
		(float)al_get_bitmap_width(data->m_double_buffer), (float)al_get_bitmap_height(data->m_double_buffer),
		0.0f, 0.0f,
		(float)al_get_bitmap_width(target), (float)al_get_bitmap_height(target),
		0);
}

static void g_app_draw_mode_transform(const g_app_data_t* data)
{
	ALLEGRO_TRANSFORM backup;
	ALLEGRO_TRANSFORM transform;
	ALLEGRO_BITMAP* target = al_get_target_bitmap();
	float x_scale = (float)al_get_bitmap_width(target) / (float)al_get_bitmap_width(data->m_double_buffer);
	float y_scale = (float)al_get_bitmap_height(target) / (float)al_get_bitmap_height(data->m_double_buffer);

	al_copy_transform(&backup, al_get_current_transform());
	al_identity_transform(&transform);
	al_scale_transform(&transform, x_scale, y_scale);
	al_compose_transform(&transform, &backup);
	al_use_transform(&transform);

	g_app_draw_inventory(data);
	
	al_use_transform(&backup);
}

static void g_app_draw(const g_app_data_t* data)
{
	void (*func_array[G_MODE_COUNT])(const g_app_data_t * data) =
	{ g_app_draw_inventory,
		g_app_draw_mode_double_buffer,
		g_app_draw_mode_transform
	};

	al_clear_to_color(G_COLOR_EIGENGRAU);

	func_array[data->m_mode](data);

	al_draw_textf(data->m_font, G_COLOR_WHITE, 0.0f, (float)al_get_display_height(data->m_display) - 22, ALLEGRO_ALIGN_LEFT, "Mode: %s", G_MODE_NAME[data->m_mode]);
	al_draw_textf(data->m_font, G_COLOR_WHITE, 0.0f, (float)al_get_display_height(data->m_display) - 12, ALLEGRO_ALIGN_LEFT, "FPS %ld", (int32_t)data->m_fps);

	al_flip_display();
}

void g_app_loop(g_app_data_t* data)
{
	double start_time = 0.0;
	double end_time = 0.0;

	al_start_timer(data->m_logic_timer);

	while (data->m_is_running)
	{
		g_app_input(data);

		if (data->m_update_logic)
		{
			data->m_update_logic = false;
			g_app_logic(data);
		}

		g_app_draw(data);

		end_time = al_get_time();
		data->m_fps = (double)(1.0 / (end_time - start_time));
		start_time = end_time;
	}

	al_stop_timer(data->m_logic_timer);
}
