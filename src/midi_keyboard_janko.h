#pragma once

#include <GL/glew.h>

#define NUM_VERTICES_PER_KEY 4
#define NUM_DIMENSIONS_PER_VERTEX 3
#define NUM_COLOR_CHANNELS_PER_VERTEX 3
#define NUM_ELEMENTS_PER_VERTEX (NUM_VERTICES_PER_KEY * NUM_DIMENSIONS_PER_VERTEX)
#define NUM_ELEMENTS_PER_COLOR (NUM_VERTICES_PER_KEY * NUM_COLOR_CHANNELS_PER_VERTEX)
#define NUM_ELEMENTS_PER_INDEX 6

typedef GLfloat midi_keyboard_key_color_t[NUM_ELEMENTS_PER_COLOR];
extern const midi_keyboard_key_color_t MIDI_KEYBOARD_COLOR_WHITE;
extern const midi_keyboard_key_color_t MIDI_KEYBOARD_COLOR_RED;

typedef struct midi_keyboard_janko {
	/* opengl data */
	GLfloat *vertices;
	GLuint *indices;
	GLfloat *colors;

	/* opengl buffers */
	GLuint vao;
	GLuint vbo_vertices;
	GLuint ebo_vertices;
	GLuint vbo_colors;

	GLuint num_keys;
	GLfloat width;
	GLfloat height;
	int midi_note_number_lowest;
} midi_keyboard_janko_t;

void midi_keyboard_janko_init(midi_keyboard_janko_t *kb, GLuint shader);
void midi_keyboard_janko_uninit(midi_keyboard_janko_t *kb);

void midi_keyboard_janko_gl_data_create(midi_keyboard_janko_t *kb);
void midi_keyboard_janko_gl_data_destroy(midi_keyboard_janko_t *kb);

void midi_keyboard_janko_gl_init(midi_keyboard_janko_t *kb, GLuint shader);
void midi_keyboard_janko_gl_uninit(midi_keyboard_janko_t *kb);

void midi_keyboard_janko_key_set_color(midi_keyboard_janko_t *kb,
				       const midi_keyboard_key_color_t *color,
				       GLuint key_id);
void midi_keyboard_janko_key_set_pressed(midi_keyboard_janko_t *kb, GLuint key_id);
void midi_keyboard_janko_key_set_unpressed(midi_keyboard_janko_t *kb, GLuint key_id);
void midi_keyboard_janko_receive_midi_note_on(midi_keyboard_janko_t *kb, int midi_note_number);
void midi_keyboard_janko_receive_midi_note_off(midi_keyboard_janko_t *kb, int midi_note_number);

void midi_keyboard_janko_render(midi_keyboard_janko_t *kb);