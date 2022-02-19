#pragma once

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
	SDL_Window *sdl_window;
	SDL_GLContext gl_context;
	bool is_running;
} window_t;

void window_sdl_print_error(void);

bool window_glew_init(void);

bool window_gl_context_create(window_t *win);

bool window_gl_context_enable_vsync(void);

bool window_sdl_window_check_gl_attribute(SDL_GLattr attr, int expected);

bool window_sdl_window_set_gl_attributes(void);

bool window_sdl_window_create(window_t *win, const char *title, int w, int h);

bool window_sdl_init(void);

bool window_create(window_t *win, const char *title, int w, int h);

void window_destroy(window_t *win);

void window_handle_events(window_t *win);

void window_update(window_t *win);

void window_run(window_t *win);