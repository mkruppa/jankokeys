#include "window.h"

#include <GL/glew.h>

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

bool window_sdl_window_set_gl_attributes(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, WINDOW_GL_CONTEXT_MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, WINDOW_GL_CONTEXT_MINOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, WINDOW_GL_CONTEXT_PROFILE_MASK);
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, WINDOW_GL_DOUBLEBUFFER);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, WINDOW_GL_CONTEXT_FLAGS);

	// check if desired sdl window attributes were set
	window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_MAJOR_VERSION, WINDOW_GL_CONTEXT_MAJOR_VERSION);
	window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_MINOR_VERSION, WINDOW_GL_CONTEXT_MINOR_VERSION);
	window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_PROFILE_MASK, WINDOW_GL_CONTEXT_PROFILE_MASK);
	// window_sdl_window_check_gl_attribute(SDL_GL_DOUBLEBUFFER, WINDOW_GL_DOUBLEBUFFER);
	window_sdl_window_check_gl_attribute(SDL_GL_CONTEXT_FLAGS, WINDOW_GL_CONTEXT_FLAGS);
	return true;
}

bool window_sdl_window_create(window_t *win, const char *title, int w, int h)
{
	win->sdl_window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		w, h,
		SDL_WINDOW_OPENGL
	);

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

bool window_create(window_t *win, const char *title, int w, int h)
{
	return win->is_running = (
		window_sdl_init() &&
		window_sdl_window_set_gl_attributes() &&
		window_sdl_window_create(win, title, w, h) &&
		window_gl_context_create(win) &&
		window_gl_context_enable_vsync() &&
		window_glew_init()
	);
}

void window_destroy(window_t *win)
{
	if (win->gl_context != NULL) { SDL_GL_DeleteContext(win->gl_context); }
	if (win->sdl_window != NULL) { SDL_DestroyWindow(win->sdl_window); }
	SDL_Quit();
}

void window_handle_events(window_t *win)
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			win->is_running = false;
			break;
		}
	}
}

void window_update(window_t *win)
{
	glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(win->sdl_window);
}

void window_run(window_t *win)
{
	while (win->is_running) {
		window_handle_events(win);
		window_update(win);
	}
}