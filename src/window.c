#include "window.h"

#include <GL/glew.h>

#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>

#include "shader.h"
#include "midi_keyboard_janko.h"
#include "midi_process_events.h"

#define WINDOW_GL_CONTEXT_MAJOR_VERSION 4
#define WINDOW_GL_CONTEXT_MINOR_VERSION 3
#define WINDOW_GL_CONTEXT_PROFILE_MASK SDL_GL_CONTEXT_PROFILE_CORE
// #define WINDOW_GL_DOUBLEBUFFER 1
#define WINDOW_GL_CONTEXT_FLAGS SDL_GL_CONTEXT_DEBUG_FLAG

void window_sdl_print_error(void)
{
	fprintf(stderr, "ERROR: SDL: %s\n", SDL_GetError());
}

bool window_glew_init(void)
{
	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	bool glew_initialized = glew_err == GLEW_OK;
	if (!glew_initialized) { fprintf(stderr, "ERROR: GLEW: %s\n", glewGetErrorString(glew_err)); }
	return glew_initialized;
}

bool window_gl_context_create(window_t *win)
{
	win->gl_context = SDL_GL_CreateContext(win->sdl_window);
	bool gl_context_created = win->gl_context != NULL;
	if (!gl_context_created) { window_sdl_print_error(); }
	return gl_context_created;
}

bool window_gl_context_enable_vsync(void)
{
	bool vsync_enabled = SDL_GL_SetSwapInterval(1) == 0;
	if (!vsync_enabled) { window_sdl_print_error(); }
	return vsync_enabled;
}

bool window_sdl_window_check_gl_attribute(SDL_GLattr attr, int expected)
{
	int val;
	if (SDL_GL_GetAttribute(attr, &val) < 0) { window_sdl_print_error(); }
	return val == expected;
}

void window_sdl_window_gl_attributes_set(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, WINDOW_GL_CONTEXT_MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, WINDOW_GL_CONTEXT_MINOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, WINDOW_GL_CONTEXT_PROFILE_MASK);
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, WINDOW_GL_DOUBLEBUFFER);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, WINDOW_GL_CONTEXT_FLAGS);
}

bool window_sdl_window_gl_attributes_check(void)
{
	return
		window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_MAJOR_VERSION, WINDOW_GL_CONTEXT_MAJOR_VERSION) &&
		window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_MINOR_VERSION, WINDOW_GL_CONTEXT_MINOR_VERSION) &&
		window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_PROFILE_MASK, WINDOW_GL_CONTEXT_PROFILE_MASK) &&
		// window_sdl_window_check_gl_attribute(SDL_GL_DOUBLEBUFFER, WINDOW_GL_DOUBLEBUFFER) &&
		window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_FLAGS, WINDOW_GL_CONTEXT_FLAGS);
}

void GLAPIENTRY window_gl_debug_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	(void)source;
	(void)type;
	(void)id;
	(void)length;
	(void)userParam;

	fprintf(stderr, "ERROR: OPENGL: %s\n", message);
	if (severity == GL_DEBUG_SEVERITY_HIGH) { abort(); }
}

void window_gl_debug_callback_enable(void)
{
	int context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(&window_gl_debug_callback, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	}
}

bool window_sdl_window_create(window_t *win, const char *title)
{
	win->sdl_window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		win->config.width, win->config.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
	);
	SDL_SetWindowOpacity(win->sdl_window, 0.5F);

	bool sdl_window_created = win->sdl_window != NULL;
	if (!sdl_window_created) { window_sdl_print_error(); }
	return sdl_window_created;
}

bool window_sdl_init(void)
{
	bool sdl_initialized = SDL_Init(SDL_INIT_VIDEO) == 0;
	if (!sdl_initialized) { window_sdl_print_error(); }
	return sdl_initialized;
}

void window_init(window_t *win)
{
	*win = (window_t){
		.sdl_window = NULL,
		.gl_context = NULL,
		.is_running = false,
		.config = (config_t){
			.width = 600,
			.height = 400
		},
		.seq = NULL,
		.port_id = -1
	};
}

bool window_create(window_t *win, const char *title)
{
	window_init(win);
	return win->is_running =
		window_config_init(win) &&
		window_midi_seq_create(win) &&
		window_sdl_init() &&
		(window_sdl_window_gl_attributes_set(), window_sdl_window_gl_attributes_check()) &&
		window_sdl_window_create(win, title) &&
		window_gl_context_create(win) &&
		window_gl_context_enable_vsync() &&
		window_glew_init() &&
		(window_gl_debug_callback_enable(), true);
}

void window_destroy(window_t *win)
{
	if (win->gl_context != NULL) { SDL_GL_DeleteContext(win->gl_context); }
	if (win->sdl_window != NULL) { SDL_DestroyWindow(win->sdl_window); }
	SDL_Quit();

	window_midi_seq_destroy(win);
}

void window_handle_events(window_t *win)
{
	SDL_Event event;
	SDL_Scancode scancode;

	int midi_note_number;
	int channel = 0;
	int velocity = 100;
	static int octave = 0;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
			scancode = event.key.keysym.scancode;
			midi_note_number = win->config.keybinding[scancode];
			if (midi_note_number != 0) { midi_note_number += octave * 12; }
		}
		switch (event.type) {
		case SDL_QUIT:
			win->is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.repeat != 0) { continue; }

			if (scancode == SDL_SCANCODE_LEFT) { --octave; break; }
			if (scancode == SDL_SCANCODE_RIGHT) { ++octave; break; }

			if (midi_note_number != 0) {
				midi_seq_event_send_note_on(win->seq, win->port_id, channel, midi_note_number, velocity);
				midi_keyboard_janko_receive_midi_note_on(&win->janko_keyboard, midi_note_number);
			}
			if (scancode == win->config.sustain_key) {
				midi_seq_event_send_sustain_off(win->seq, win->port_id);
			}
			break;
		case SDL_KEYUP:
			if (midi_note_number != 0) {
				midi_seq_event_send_note_off(win->seq, win->port_id, channel, midi_note_number, velocity);
				midi_keyboard_janko_receive_midi_note_off(&win->janko_keyboard, midi_note_number);
			}
			if (scancode == win->config.sustain_key) {
				midi_seq_event_send_sustain_on(win->seq, win->port_id);
			}
			break;
		}
	}
}

void window_update(window_t *win, double run_time_ms, double delta_time_ms)
{
	glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(win->shader);

	midi_keyboard_janko_update_keys(&win->janko_keyboard, delta_time_ms);
	midi_keyboard_janko_render(&win->janko_keyboard, &win->MVP);

	piano_roll_update(&win->piano_roll, run_time_ms);
	piano_roll_render(&win->piano_roll,  &win->MVP);

	SDL_GL_SwapWindow(win->sdl_window);
}

void window_run(window_t *win)
{
	win->shader = shader_program_create_from_file("../shader/simple.vert", "../shader/simple.frag");
	window_gl_origin_set_bottom_left(win);

	texture_atlas_create(&win->texture_atlas, "../img/spritesheet.png");
	texture_atlas_set_tile_size(&win->texture_atlas, 32, 48);

	midi_keyboard_janko_init(&win->janko_keyboard, &win->texture_atlas, win->shader, win->config.width, win->config.height, win->config.janko_num_rows, MIDI_NOTE_A0);

	piano_roll_init(&win->piano_roll,
			&win->texture_atlas,
			win->shader,
			win->janko_keyboard.num_rows * midi_keyboard_janko_key_height(&win->janko_keyboard),
			midi_keyboard_janko_key_width(&win->janko_keyboard),
			MIDI_NOTE_A0);

	sdl_clock_start(&win->clock);
	sdl_tick_t tick_prev = sdl_clock_elapsed(&win->clock);
	while (win->is_running) {
		sdl_tick_t tick_now = sdl_clock_elapsed(&win->clock);
		double run_time_ms = sdl_clock_tick_to_ms(tick_now);
		double delta_time_ms = sdl_clock_tick_to_ms(tick_now - tick_prev);
		tick_prev = tick_now;

		window_update(win, run_time_ms, delta_time_ms);
		window_handle_events(win);
		midi_process_events(win, run_time_ms);
	}

	piano_roll_uninit(&win->piano_roll);
	midi_keyboard_janko_uninit(&win->janko_keyboard);
	texture_atlas_destroy(&win->texture_atlas);
	shader_program_destroy(win->shader);
}

void window_gl_origin_set_bottom_left(window_t *win)
{
	glm_ortho(0.0F, win->config.width, 0.0F, win->config.height, 0.0F, 100.0F, win->MVP);
	glUseProgram(win->shader);
	GLint loc = glGetUniformLocation(win->shader, "M");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)win->MVP);
}

bool window_config_init(window_t *win)
{
	return config_load(&win->config);
}

bool window_midi_seq_create(window_t *win)
{
	/* initialize alsa midi sequencer */
	win->seq = midi_seq_client_open("jankokeys");
	if (win->seq == NULL) {
		fprintf(stderr, "ERROR: ALSA: couldn't open sequencer client\n");
		return false;
	}

	win->port_id = midi_seq_port_create(win->seq, "midi_port");
	if (win->port_id < 0) {
		fprintf(stderr, "ERROR: ALSA: couldn't open sequencer port\n");
		return false;
	}

	/* connect to the midi through port */
	if (midi_seq_connect_to(win->seq, win->port_id, MIDI_THROUGH_CLIENT_ID, MIDI_THROUGH_PORT_ID) < 0) {
		fprintf(stderr, "ERROR: ALSA: couldn't connect to midi through port %d:%d\n",
			MIDI_THROUGH_CLIENT_ID, MIDI_THROUGH_PORT_ID);
		return false;
	}

	if (snd_seq_connect_from(win->seq, win->port_id, MIDI_THROUGH_CLIENT_ID, MIDI_THROUGH_PORT_ID) < 0) {
		fprintf(stderr, "ERROR: ALSA: couldn't connect from midi through port %d:%d\n",
			MIDI_THROUGH_CLIENT_ID, MIDI_THROUGH_PORT_ID);
		return false;
	}

	return true;
}

void window_midi_seq_destroy(window_t *win)
{
	if (win->port_id >= 0) { midi_seq_port_destroy(win->seq, win->port_id); }
	if (win->seq != NULL) { midi_seq_client_close(win->seq); }
}