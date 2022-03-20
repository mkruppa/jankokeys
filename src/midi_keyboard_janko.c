#include "midi_keyboard_janko.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIDI_NOTE_C4 60
#define MIDI_NOTE_A0 21

#define SIZE_VERTEX (NUM_ELEMENTS_PER_VERTEX * sizeof(GLfloat))
#define SIZE_INDEX (NUM_ELEMENTS_PER_INDEX * sizeof(GLuint))
#define SIZE_COLOR (NUM_ELEMENTS_PER_COLOR * sizeof(GLfloat))
#define SIZE_TEXTURE_UV (NUM_ELEMENTS_PER_TEXTURE_UV * sizeof(GLfloat))

#define JANKO_KEY_HEIGHT_OVER_WIDTH_RATIO (3.0F / 2.0F)
#define MIDI_KEYBOARD_JANKO_MIN_NUM_ROWS 2
#define MIDI_KEYBOARD_JANKO_MAX_NUM_ROWS 6

static uv_quad_t uv_key_black;
static uv_quad_t uv_key_white;

#define NUM_KEYS_PER_OCTAVE 12
#define NUM_JANKO_KEYBOARD_ROWS 2
#define NUM_KEYS_PER_OCTAVE_PER_ROW (NUM_KEYS_PER_OCTAVE / NUM_JANKO_KEYBOARD_ROWS)
static int key_types_f_row[NUM_KEYS_PER_OCTAVE_PER_ROW] = { 1, 1, 0, 0, 0, 0 };
static int key_types_c_row[NUM_KEYS_PER_OCTAVE_PER_ROW] = { 0, 0, 0, 1, 1, 1 };
static int *key_types_top_row = key_types_f_row;
static int *key_types_bottom_row = key_types_c_row;

#define MIDI_KEYBOARD_KEY_PRESSED -1
#define MIDI_KEYBOARD_KEY_RELEASED_MS 300
static midi_keyboard_key_color_t key_color_pressed;
static midi_keyboard_key_color_t key_color_unpressed;

static bool is_bottom_row_indented;

static int midi_distance_from_c(int midi_note_number)
{
	return midi_note_number % NUM_KEYS_PER_OCTAVE; // midi note number 0 is a c key
}

static bool is_lowest_midi_note_in_f_row(midi_keyboard_janko_t *kb)
{
	return midi_distance_from_c(kb->midi_note_number_lowest) % NUM_JANKO_KEYBOARD_ROWS;
}

void midi_keyboard_janko_init(midi_keyboard_janko_t *kb, GLuint shader, int width, int num_rows)
{
	*kb = (midi_keyboard_janko_t){
		.width = width,
		.midi_note_number_lowest = MIDI_NOTE_A0,
		.shader_program = shader,
		.num_rows = num_rows,
	};
	assert(kb->num_rows >= MIDI_KEYBOARD_JANKO_MIN_NUM_ROWS);
	assert(kb->num_rows <= MIDI_KEYBOARD_JANKO_MAX_NUM_ROWS);

	midi_keyboard_key_color_rgb(0.2F, 0.3F, 0.8F, key_color_pressed);
	midi_keyboard_key_color_rgb(1.0F, 1.0F, 1.0F, key_color_unpressed);

	is_bottom_row_indented = is_lowest_midi_note_in_f_row(kb);

	texture_atlas_load("../img/spritesheet.png", &kb->texture_atlas);
	kb->texture_atlas.tw = 32;
	kb->texture_atlas.th = 48;
	texture_atlas_get_uv_quad(&kb->texture_atlas, 0, 0, uv_key_black);
	texture_atlas_get_uv_quad(&kb->texture_atlas, 1, 0, uv_key_white);

	kb->pressed_keys = calloc(NUM_MIDI_KEYBOARD_KEYS, sizeof(*kb->pressed_keys));
	midi_keyboard_janko_gl_data_create(kb);
	midi_keyboard_janko_gl_init(kb, shader);
}

void midi_keyboard_janko_uninit(midi_keyboard_janko_t *kb)
{
	glDeleteTextures(1, &kb->texture_atlas.texture_id);
	free(kb->pressed_keys);
	midi_keyboard_janko_gl_data_destroy(kb);
	midi_keyboard_janko_gl_uninit(kb);
}

uv_quad_t *midi_keyboard_janko_key_uv(midi_keyboard_janko_t *kb, GLuint key_id)
{
	const div_t row_distance = div(midi_distance_from_c(kb->midi_note_number_lowest), NUM_JANKO_KEYBOARD_ROWS);
	const int offset = row_distance.quot + row_distance.rem;
	const int offset_top_row = -row_distance.rem;

	const GLuint num_keys_bottom_row = midi_keyboard_janko_num_keys_row_bottom(kb);
	int key_type;
	if (key_id >= num_keys_bottom_row) { // top row key
		key_type = key_types_top_row[(key_id - num_keys_bottom_row + offset + offset_top_row) % NUM_KEYS_PER_OCTAVE_PER_ROW];
	} else {
		key_type = key_types_bottom_row[(key_id + offset) % NUM_KEYS_PER_OCTAVE_PER_ROW];
	}

	return key_type ? &uv_key_black : &uv_key_white;
}

void midi_keyboard_janko_gl_data_create(midi_keyboard_janko_t *kb)
{
	/*
		For 1 octave consisting of 2 janko rows, the janko key ids (in hexadecimal) in the vertices vbo are set to
			 6 7 8 9 A B	<-- top row
			0 1 2 3 4 5	<-- bottom row
		which also makes it easy to render only one of the rows (for example for a 3 row janko keyboard).

		Each janko key consists of 2 triangles with ccw winding:
			2───3
			│ \ │	1st triangle: 0 1 2 (lower left)
			0───1	2nd triangle: 2 1 3 (upper right)

		The corresponding midi note numbers (in hexadecimal) for the janko keys are (if bottom row is not indented):
			 1 3 5 7 9 B	<-- odd row (C#, D#, F, G, A, B)
			0 2 4 6 8 A	<-- even row (C, D, E, F#, G#, A#)
	*/
	kb->vertices = calloc(NUM_MIDI_KEYBOARD_KEYS, SIZE_VERTEX);
	kb->indices = calloc(NUM_MIDI_KEYBOARD_KEYS, SIZE_INDEX);
	kb->colors = calloc(NUM_MIDI_KEYBOARD_KEYS, SIZE_COLOR);
	kb->texture_uvs = calloc(NUM_MIDI_KEYBOARD_KEYS, SIZE_TEXTURE_UV);

	const GLuint num_keys_bottom_row = midi_keyboard_janko_num_keys_row_bottom(kb);
	const GLfloat key_width = floor(midi_keyboard_janko_key_width(kb));
	const GLfloat key_height = floor(key_width * JANKO_KEY_HEIGHT_OVER_WIDTH_RATIO);

	for (size_t i = 0; i < NUM_MIDI_KEYBOARD_KEYS; ++i) {
		GLfloat y1 = 0.0F;
		GLfloat y2 = key_height;

		bool is_top_row_key = i >= num_keys_bottom_row;
		GLfloat k = i;
		if (is_top_row_key) {
			k -= num_keys_bottom_row;
			y1 += key_height;
			y2 += key_height;
		}
		if (is_bottom_row_indented) {
			k += is_top_row_key ? 0.0F : 0.5F;
		} else {
			k += is_top_row_key ? 0.5F : 0.0F;
		}

		GLfloat x1 = k * key_width;
		GLfloat x2 = x1 + key_width;

		GLfloat vertex[] = {
			x1, y1, 0.0F,
			x2, y1, 0.0F,
			x1, y2, 0.0F,
			x2, y2, 0.0F,
		};

		GLuint j = i * NUM_VERTICES_PER_KEY;
		GLuint index[] = {
			j + 0, j + 1, j + 2,
			j + 2, j + 1, j + 3,
		};

		uv_quad_t *uv_key = midi_keyboard_janko_key_uv(kb, i);

		/* append the data */
		memcpy(&kb->vertices[i * NUM_ELEMENTS_PER_VERTEX], vertex, SIZE_VERTEX);
		memcpy(&kb->indices[i * NUM_ELEMENTS_PER_INDEX], index, SIZE_INDEX);
		memcpy(&kb->colors[i * NUM_ELEMENTS_PER_COLOR], key_color_unpressed, SIZE_COLOR);
		memcpy(&kb->texture_uvs[i * NUM_ELEMENTS_PER_TEXTURE_UV], uv_key, SIZE_TEXTURE_UV);
	}
}

void midi_keyboard_janko_gl_data_destroy(midi_keyboard_janko_t *kb)
{
	free(kb->vertices);
	free(kb->indices);
	free(kb->colors);
	free(kb->texture_uvs);
}

void midi_keyboard_janko_gl_init(midi_keyboard_janko_t *kb, GLuint shader)
{
	glGenVertexArrays(1, &kb->vao);
	glBindVertexArray(kb->vao);

	/* vertices */
	glGenBuffers(1, &kb->vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, NUM_MIDI_KEYBOARD_KEYS * SIZE_VERTEX, kb->vertices, GL_STATIC_DRAW);

	GLint pos = glGetAttribLocation(shader, "vert_pos");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, NUM_DIMENSIONS_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

	/* indices */
	glGenBuffers(1, &kb->ebo_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kb->ebo_vertices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_MIDI_KEYBOARD_KEYS * SIZE_INDEX, kb->indices, GL_STATIC_DRAW);

	/* colors */
	glGenBuffers(1, &kb->vbo_colors);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo_colors);
	glBufferData(GL_ARRAY_BUFFER, NUM_MIDI_KEYBOARD_KEYS * SIZE_COLOR, kb->colors, GL_DYNAMIC_DRAW);

	GLint col = glGetAttribLocation(shader, "vert_col");
	glEnableVertexAttribArray(col);
	glVertexAttribPointer(col, NUM_COLOR_CHANNELS_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

	/* texture uvs */
	glGenBuffers(1, &kb->vbo_texture_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo_texture_uvs);
	glBufferData(GL_ARRAY_BUFFER, NUM_MIDI_KEYBOARD_KEYS * SIZE_TEXTURE_UV, kb->texture_uvs, GL_STATIC_DRAW);

	GLint tex = glGetAttribLocation(kb->shader_program, "vert_tex");
	glEnableVertexAttribArray(tex);
	glVertexAttribPointer(tex, NUM_DIMENSIONS_PER_UV, GL_FLOAT, GL_FALSE, 0, NULL);

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
	glDeleteBuffers(1, &kb->vbo_texture_uvs);
}

GLuint midi_keyboard_janko_key_id(midi_keyboard_janko_t *kb, GLuint midi_key_id)
{
	GLuint key_id = midi_key_id / 2;
	if (midi_keyboard_janko_is_top_row_key(midi_key_id)) {
		key_id += midi_keyboard_janko_num_keys_row_bottom(kb);
	}
	return key_id;
}

bool midi_keyboard_janko_is_top_row_key(GLuint midi_key_id)
{
	bool is_midi_key_id_odd = midi_key_id % 2;
	return !is_bottom_row_indented ? is_midi_key_id_odd : !is_midi_key_id_odd;
}

void midi_keyboard_janko_update_key_color(midi_keyboard_janko_t *kb, size_t midi_key_id, double delta_time_ms, midi_keyboard_key_color_t color)
{
	if (midi_keyboard_janko_is_key_pressed(kb, midi_key_id)) {
		memcpy(color, key_color_pressed, SIZE_COLOR);
	} else if (kb->pressed_keys[midi_key_id] > 0) {
		kb->pressed_keys[midi_key_id] = fmax(0, kb->pressed_keys[midi_key_id] - delta_time_ms);
		midi_keyboard_key_color_ease(
			MIDI_KEYBOARD_KEY_RELEASED_MS - kb->pressed_keys[midi_key_id],
			MIDI_KEYBOARD_KEY_RELEASED_MS,
			key_color_pressed,
			key_color_unpressed, 
			color);
	} else {
		memcpy(color, key_color_unpressed, SIZE_COLOR);
	}
}

void midi_keyboard_janko_update_keys(midi_keyboard_janko_t *kb, double delta_time_ms)
{
	for(size_t midi_key_id = 0; midi_key_id < NUM_MIDI_KEYBOARD_KEYS; ++midi_key_id) {
		midi_keyboard_key_color_t color;
		midi_keyboard_janko_update_key_color(kb, midi_key_id, delta_time_ms, color);
		memcpy(&kb->colors[midi_keyboard_janko_key_id(kb, midi_key_id) * NUM_ELEMENTS_PER_COLOR], color, SIZE_COLOR);
	}

	glBindVertexArray(kb->vao);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo_colors);
	glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_MIDI_KEYBOARD_KEYS * SIZE_COLOR, kb->colors);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool midi_keyboard_janko_is_key_pressed(midi_keyboard_janko_t *kb, GLuint midi_key_id)
{
	return kb->pressed_keys[midi_key_id] == MIDI_KEYBOARD_KEY_PRESSED;
}

void midi_keyboard_janko_set_key_pressed(midi_keyboard_janko_t *kb, GLuint midi_key_id)
{
	if (midi_key_id < NUM_MIDI_KEYBOARD_KEYS) { kb->pressed_keys[midi_key_id] = MIDI_KEYBOARD_KEY_PRESSED; }
}

void midi_keyboard_janko_set_key_unpressed(midi_keyboard_janko_t *kb, GLuint midi_key_id)
{
	if (midi_key_id < NUM_MIDI_KEYBOARD_KEYS) { kb->pressed_keys[midi_key_id] = MIDI_KEYBOARD_KEY_RELEASED_MS; }
}

void midi_keyboard_janko_receive_midi_note_on(midi_keyboard_janko_t *kb, int midi_note_number)
{
	midi_keyboard_janko_set_key_pressed(kb, midi_note_number - kb->midi_note_number_lowest);
}

void midi_keyboard_janko_receive_midi_note_off(midi_keyboard_janko_t *kb, int midi_note_number)
{
	midi_keyboard_janko_set_key_unpressed(kb, midi_note_number - kb->midi_note_number_lowest);
}

GLfloat midi_keyboard_janko_key_width(midi_keyboard_janko_t *kb)
{
	return (kb->width / (NUM_MIDI_KEYBOARD_KEYS + 1U)) * 2.0F;
}

GLuint midi_keyboard_janko_num_keys_row_bottom()
{
	return (NUM_MIDI_KEYBOARD_KEYS + 1U) / 2U;
}

void midi_keyboard_janko_render(midi_keyboard_janko_t *kb, mat4 *MVP)
{
	glBindVertexArray(kb->vao);

	GLint loc = glGetUniformLocation(kb->shader_program, "M");

	mat4 mat;
	glm_mat4_copy(*MVP, mat);

	/* draw rows 1 and 2 */
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)mat);
	glDrawElements(GL_TRIANGLES, NUM_MIDI_KEYBOARD_KEYS * NUM_ELEMENTS_PER_INDEX, GL_UNSIGNED_INT, 0);

	/* draw remaining rows */
	uint num_remaining_rows = kb->num_rows - MIDI_KEYBOARD_JANKO_MIN_NUM_ROWS;
	while(num_remaining_rows > 0) {
		const GLfloat key_height = midi_keyboard_janko_key_width(kb) * JANKO_KEY_HEIGHT_OVER_WIDTH_RATIO;
		glm_translate(mat, (vec4){0.0F, MIDI_KEYBOARD_JANKO_MIN_NUM_ROWS * key_height, 0.0F, 0.0F});
		glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)mat);

		if (num_remaining_rows == 1) {
			glDrawElements(GL_TRIANGLES, midi_keyboard_janko_num_keys_row_bottom() * NUM_ELEMENTS_PER_INDEX, GL_UNSIGNED_INT, 0);
			num_remaining_rows -= 1;
		} else {
			glDrawElements(GL_TRIANGLES, NUM_MIDI_KEYBOARD_KEYS * NUM_ELEMENTS_PER_INDEX, GL_UNSIGNED_INT, 0);
			num_remaining_rows -= 2;
		}
	}

	glBindVertexArray(0);
}