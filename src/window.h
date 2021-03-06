#pragma once

#include <stdbool.h>
#include <SDL.h>

#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>

#include "config.h"
#include "alsa_seq.h"
#include "midi_keyboard_janko.h"
#include "sdl_clock.h"
#include "piano_roll.h"

typedef struct window {
	/* sdl opengl */
	SDL_Window *sdl_window;
	SDL_GLContext gl_context;
	bool is_running;
	sdl_clock_t clock;

	/* lua */
	config_t config;

	/* alsa */
	midi_seq_t *seq;
	int port_id;

	/* opengl */
	mat4 MVP;
	GLuint shader;
	texture_atlas_t texture_atlas;
	midi_keyboard_janko_t janko_keyboard;
	piano_roll_t piano_roll;
} window_t;

void window_sdl_print_error(void);

bool window_glew_init(void);
bool window_gl_context_create(window_t *win);
bool window_gl_context_enable_vsync(void);

bool window_sdl_window_check_gl_attribute(SDL_GLattr attr, int expected);
void window_sdl_window_gl_attributes_set(void);
bool window_sdl_window_gl_attributes_check(void);

void window_gl_debug_callback_enable(void);

bool window_sdl_window_create(window_t *win, const char *title);
bool window_sdl_init(void);

void window_init(window_t *win);
bool window_create(window_t *win, const char *title);
void window_destroy(window_t *win);

void window_handle_events(window_t *win);
void window_update(window_t *win, double run_time_ms, double delta_time_ms);
void window_run(window_t *win);

void window_gl_origin_set_bottom_left(window_t *win);
bool window_config_init(window_t *win);

bool window_midi_seq_create(window_t *win);
void window_midi_seq_destroy(window_t *win);