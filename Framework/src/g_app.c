#include <allegro5/allegro5.h>

typedef struct g_point_t
{
	float m_width;
	float m_height;
} g_point_t;

typedef struct g_app_data_t
{
	ALLEGRO_DISPLAY* m_display;
	ALLEGRO_TIMER* m_logic_timer;
	ALLEGRO_EVENT_QUEUE* m_event_queue;
	bool m_is_running;
	bool m_update_logic;
} g_app_data_t;

static const char* const G_TITLE = "Framework!";
static const g_point_t G_DISPLAY_INITIAL_SIZE = { 800.0f, 600.0f };
static const g_point_t G_GAME_SCREEN_SIZE = { 800.0f, 600.0f };
static const double G_LOGIC_TIMER_RATE = 60.0;
static const ALLEGRO_COLOR G_COLOR_EIGENGRAU = { 0.08627451f, 0.08627451f, 0.11372549f, 1.0f };

int32_t g_app_initialize();
void g_app_shutdown();
void g_app_loop();

static g_app_data_t g_data;

int32_t main(int32_t argc, char** argv)
{
	if (g_app_initialize() == 0)
	{
		g_app_loop();
	}

	g_app_shutdown();

	return 0;
}

static void g_app_zero_initialize_data()
{
	g_data.m_display = NULL;
	g_data.m_logic_timer = NULL;
	g_data.m_event_queue = NULL;
	g_data.m_is_running = true;
	g_data.m_update_logic = false;
}

int32_t g_app_initialize()
{
	g_app_zero_initialize_data();

	if (!al_init())
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
	if (!(g_data.m_display = al_create_display((int32_t)G_DISPLAY_INITIAL_SIZE.m_width, (int32_t)G_DISPLAY_INITIAL_SIZE.m_height)))
	{
		return -1;
	}

	if (!(g_data.m_logic_timer = al_create_timer(1.0 / G_LOGIC_TIMER_RATE)))
	{
		return -1;
	}

	if (!(g_data.m_event_queue = al_create_event_queue()))
	{
		return -1;
	}

	al_register_event_source(g_data.m_event_queue, al_get_display_event_source(g_data.m_display));
	al_register_event_source(g_data.m_event_queue, al_get_timer_event_source(g_data.m_logic_timer));
	al_register_event_source(g_data.m_event_queue, al_get_keyboard_event_source());
	al_register_event_source(g_data.m_event_queue, al_get_mouse_event_source());

	return 0;
}

void g_app_shutdown()
{
	if (g_data.m_event_queue)
	{
		al_destroy_event_queue(g_data.m_event_queue);
		g_data.m_event_queue = NULL;
	}

	if (g_data.m_logic_timer)
	{
		al_destroy_timer(g_data.m_logic_timer);
		g_data.m_logic_timer = NULL;
	}

	if (g_data.m_display)
	{
		al_destroy_display(g_data.m_display);
		g_data.m_display = NULL;
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

static void g_app_input()
{
	static ALLEGRO_EVENT event;

	while (!al_is_event_queue_empty(g_data.m_event_queue))
	{
		al_get_next_event(g_data.m_event_queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
		{
			g_data.m_update_logic = true;
		} break;

		case ALLEGRO_EVENT_KEY_DOWN:
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				g_data.m_is_running = false;
			}
		} break;

		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			g_data.m_is_running = false;
		} break;
		default: return;
		}
	}
}

static void g_app_logic()
{
	// empty
}

static void g_app_draw()
{
	al_clear_to_color(G_COLOR_EIGENGRAU);

	al_flip_display();
}

void g_app_loop()
{
	al_start_timer(g_data.m_logic_timer);

	while (g_data.m_is_running)
	{
		g_app_input();

		if (g_data.m_update_logic)
		{
			g_data.m_update_logic = false;
			g_app_logic();
		}

		g_app_draw();
	}

	al_stop_timer(g_data.m_logic_timer);
}
