#include "midi_keyboard_janko.h"
#include "alsa_seq.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* key color definitions */
const midi_keyboard_key_color_t MIDI_KEYBOARD_COLOR_WHITE = {
	1.0F, 1.0F, 1.0F,
	1.0F, 1.0F, 1.0F,
	1.0F, 1.0F, 1.0F,
	1.0F, 1.0F, 1.0F,
};
const midi_keyboard_key_color_t MIDI_KEYBOARD_COLOR_RED = {
	1.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,
	1.0F, 0.0F, 0.0F,
};

void midi_keyboard_janko_init(midi_keyboard_janko_t *kb, GLuint shader)
{
	*kb = (midi_keyboard_janko_t){
		.num_keys = 25,
		.width = 1.0F,
		.height = 0.2F,
		.midi_note_number_lowest = MIDI_NOTE_MIDDLE_C - 2,
	};

	midi_keyboard_janko_gl_data_create(kb);
	midi_keyboard_janko_gl_init(kb, shader);
	midi_keyboard_janko_gl_data_destroy(kb);
}

void midi_keyboard_janko_uninit(midi_keyboard_janko_t *kb)
{
	midi_keyboard_janko_gl_uninit(kb);
}

void midi_keyboard_janko_gl_data_create(midi_keyboard_janko_t *kb)
{
	GLfloat key_width = kb->width / kb->num_keys;

	kb->vertices = malloc(kb->num_keys * NUM_ELEMENTS_PER_VERTEX * sizeof(GLfloat));
	kb->indices = malloc(kb->num_keys * NUM_ELEMENTS_PER_INDEX * sizeof(GLuint));
	kb->colors = malloc(kb->num_keys * NUM_ELEMENTS_PER_COLOR * sizeof(GLfloat));

	for (GLuint i = 0; i < kb->num_keys; ++i) {
		/*
			2───3	CCW winding:
			│ \ │		1st triangle (lower left): 0 1 2
			0───1		2nd triangle (upper right): 2 1 3
		*/
		GLfloat x1 = i * key_width;
		GLfloat x2 = x1 + key_width;
		GLfloat vertex[] = {
			x1, 0.0F,       0.0F,
			x2, 0.0F,       0.0F,
			x1, kb->height, 0.0F,
			x2, kb->height, 0.0F,
		};
		GLuint j = i * NUM_VERTICES_PER_KEY;
		GLuint index[] = {
			j + 0, j + 1, j + 2,
			j + 2, j + 1, j + 3,
		};

		memcpy(kb->vertices + i * NUM_ELEMENTS_PER_VERTEX, vertex, sizeof(vertex));
		memcpy(kb->indices + i * NUM_ELEMENTS_PER_INDEX, index, sizeof(index));
		memcpy(
			kb->colors + i * NUM_ELEMENTS_PER_COLOR,
			MIDI_KEYBOARD_COLOR_WHITE,
			// i % 2 == 0 ? MIDI_KEYBOARD_COLOR_WHITE : MIDI_KEYBOARD_COLOR_RED,
			sizeof(MIDI_KEYBOARD_COLOR_WHITE)
		);
	}
}

void midi_keyboard_janko_gl_data_destroy(midi_keyboard_janko_t *kb)
{
	free(kb->vertices);
	free(kb->indices);
	free(kb->colors);
}

void midi_keyboard_janko_gl_init(midi_keyboard_janko_t *kb, GLuint shader)
{
	const size_t vertices_size = kb->num_keys * NUM_ELEMENTS_PER_VERTEX * sizeof(GLfloat);
	const size_t indices_size = kb->num_keys * NUM_ELEMENTS_PER_INDEX * sizeof(GLuint);
	const size_t colors_size = kb->num_keys * NUM_ELEMENTS_PER_COLOR * sizeof(GLfloat);

	glGenVertexArrays(1, &kb->vao);
	glBindVertexArray(kb->vao);

	/* vertices */
	glGenBuffers(1, &kb->vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, kb->vertices, GL_STATIC_DRAW);

	GLint pos = glGetAttribLocation(shader, "vert_pos");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	/* indices */
	glGenBuffers(1, &kb->ebo_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kb->ebo_vertices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, kb->indices, GL_STATIC_DRAW);

	/* colors */
	glGenBuffers(1, &kb->vbo_colors);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo_colors);
	glBufferData(GL_ARRAY_BUFFER, colors_size, kb->colors, GL_DYNAMIC_DRAW);

	GLint col = glGetAttribLocation(shader, "vert_col");
	glEnableVertexAttribArray(col);
	glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void midi_keyboard_janko_gl_uninit(midi_keyboard_janko_t *kb)
{
	glDeleteVertexArrays(1, &kb->vao);
	glDeleteBuffers(1, &kb->vbo_vertices);
	glDeleteBuffers(1, &kb->ebo_vertices);
	glDeleteBuffers(1, &kb->vbo_colors);
}

void midi_keyboard_janko_key_set_color(
	midi_keyboard_janko_t *kb,
	const midi_keyboard_key_color_t *color,
	GLuint key_id)
{
	assert(key_id < kb->num_keys);

	glBindVertexArray(kb->vao);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo_colors);

	glBufferSubData(
		GL_ARRAY_BUFFER,
		key_id * sizeof(MIDI_KEYBOARD_COLOR_WHITE),
		sizeof(MIDI_KEYBOARD_COLOR_WHITE),
		*color
	);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void midi_keyboard_janko_key_set_pressed(midi_keyboard_janko_t *kb, GLuint key_id)
{
	assert(key_id < kb->num_keys);
	midi_keyboard_janko_key_set_color(kb, &MIDI_KEYBOARD_COLOR_RED, key_id);
}

void midi_keyboard_janko_key_set_unpressed(midi_keyboard_janko_t *kb, GLuint key_id)
{
	assert(key_id < kb->num_keys);
	midi_keyboard_janko_key_set_color(kb, &MIDI_KEYBOARD_COLOR_WHITE, key_id);
}

void midi_keyboard_janko_receive_midi_note_on(midi_keyboard_janko_t *kb, int midi_note_number)
{
	midi_keyboard_janko_key_set_pressed(kb, midi_note_number - kb->midi_note_number_lowest);
}

void midi_keyboard_janko_receive_midi_note_off(midi_keyboard_janko_t *kb, int midi_note_number)
{
	midi_keyboard_janko_key_set_unpressed(kb, midi_note_number - kb->midi_note_number_lowest);
}

void midi_keyboard_janko_render(midi_keyboard_janko_t *kb)
{
	glBindVertexArray(kb->vao);
	glDrawElements(GL_TRIANGLES, kb->num_keys * NUM_ELEMENTS_PER_INDEX, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}