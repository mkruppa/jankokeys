#pragma once

#include <GL/glew.h>

#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>

#include "midi_common.h"
#include "midi_keyboard_key_color.h"
#include "texture_atlas.h"

#define MIDI_NOTE_C4 60
#define MIDI_NOTE_A0 21

typedef struct midi_keyboard_janko {
	/* opengl data */
	GLfloat *vertices;
	GLuint *indices;
	GLfloat *colors;
	GLfloat *texture_uvs;

	/* opengl buffers */
	GLuint vao;
	GLuint vbo_vertices;
	GLuint ebo_vertices;
	GLuint vbo_colors;
	GLuint vbo_texture_uvs;

	GLuint shader_program;

	GLfloat width;
	GLfloat height;
	uint num_rows;
	int midi_note_number_lowest;
	int *pressed_keys;
} midi_keyboard_janko_t;

void midi_keyboard_janko_init(midi_keyboard_janko_t *kb, texture_atlas_t *texture_atlas, GLuint shader, int width, int height, int num_rows, int midi_note_number_lowest);
void midi_keyboard_janko_uninit(midi_keyboard_janko_t *kb);

uv_quad_t *midi_keyboard_janko_key_uv(midi_keyboard_janko_t *kb, GLuint key_id);
void midi_keyboard_janko_gl_data_create(midi_keyboard_janko_t *kb);
void midi_keyboard_janko_gl_data_destroy(midi_keyboard_janko_t *kb);

void midi_keyboard_janko_gl_init(midi_keyboard_janko_t *kb, GLuint shader);
void midi_keyboard_janko_gl_uninit(midi_keyboard_janko_t *kb);

GLuint midi_keyboard_janko_key_id(midi_keyboard_janko_t *kb, GLuint midi_key_id);
bool midi_keyboard_janko_is_top_row_key(GLuint midi_key_id);

void midi_keyboard_janko_update_key_color(midi_keyboard_janko_t *kb, size_t midi_key_id, double delta_time_ms, midi_keyboard_key_color_t color);
void midi_keyboard_janko_update_keys(midi_keyboard_janko_t *kb, double delta_time_ms);
void midi_keyboard_janko_reset_keys(midi_keyboard_janko_t *kb);

bool midi_keyboard_janko_is_key_pressed(midi_keyboard_janko_t *kb, GLuint midi_key_id);
void midi_keyboard_janko_set_key_pressed(midi_keyboard_janko_t *kb, GLuint midi_key_id);
void midi_keyboard_janko_set_key_unpressed(midi_keyboard_janko_t *kb, GLuint midi_key_id);

void midi_keyboard_janko_receive_midi_note_on(midi_keyboard_janko_t *kb, int midi_note_number);
void midi_keyboard_janko_receive_midi_note_off(midi_keyboard_janko_t *kb, int midi_note_number);

GLfloat midi_keyboard_janko_key_width(midi_keyboard_janko_t *kb);
GLfloat midi_keyboard_janko_key_height(midi_keyboard_janko_t *kb);
GLuint midi_keyboard_janko_num_keys_row_bottom();

void midi_keyboard_janko_render(midi_keyboard_janko_t *kb, mat4 *MVP);