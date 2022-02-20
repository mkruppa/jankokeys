#pragma once

#include <GL/glew.h>

typedef struct midi_keyboard_janko {
	GLuint vao;
	GLuint vbo;
} midi_keyboard_janko_t;

void midi_keyboard_janko_init(midi_keyboard_janko_t *kb, GLuint shader);

void midi_keyboard_janko_render(midi_keyboard_janko_t *kb);