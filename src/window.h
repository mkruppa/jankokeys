#pragma once

#include <stdbool.h>
#include <SDL.h>

#include "config.h"
#include "alsa_seq.h"

typedef struct {
	SDL_Window *sdl_window;
	SDL_GLContext gl_context;
	bool is_running;

	config_t config;

	midi_seq_t *seq;
	int port_id;
} window_t;

void window_sdl_print_error(void);

bool window_glew_init(void);

bool window_gl_context_create(window_t *win);

bool window_gl_context_enable_vsync(void);

bool window_sdl_window_check_gl_attribute(SDL_GLattr attr, int expected);

void window_sdl_window_gl_attributes_set(void);

bool window_sdl_window_gl_attributes_check(void);

bool window_sdl_window_create(window_t *win, const char *title);

bool window_sdl_init(void);

void window_init(window_t *win);

bool window_create(window_t *win, const char *title);

void window_destroy(window_t *win);

void window_handle_events(window_t *win);

void window_update(window_t *win);

void window_run(window_t *win);

bool window_config_init(window_t *win);

bool window_midi_seq_create(window_t *win);

void window_midi_seq_destroy(window_t *win);