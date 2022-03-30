#pragma once

#include <GL/gl.h>
#include <sys/types.h>
#include <cglm/types.h>

#include "midi_common.h"
#include "texture_atlas.h"

#define MAX_NUM_DISPLAYED_NOTES 256
#define NUM_MIDI_NOTE_NUMBERS 128

#define NUM_ELEMENTS_VERTICES (MAX_NUM_DISPLAYED_NOTES * NUM_ELEMENTS_PER_VERTEX)
#define NUM_ELEMENTS_INDICES (MAX_NUM_DISPLAYED_NOTES * NUM_ELEMENTS_PER_INDEX)
#define NUM_ELEMENTS_TEXTURE_UVS (MAX_NUM_DISPLAYED_NOTES * NUM_ELEMENTS_PER_TEXTURE_UV)
#define NUM_ELEMENTS_COLORS (MAX_NUM_DISPLAYED_NOTES * NUM_ELEMENTS_PER_COLOR)

typedef struct piano_roll_note {
	double time;
	double duration;

	u_char channel;
	u_char note;
	u_char velocity;
} piano_roll_note_t;

typedef struct piano_roll {
	/* opengl data */
	GLfloat vertices[NUM_ELEMENTS_VERTICES];
	GLuint indices[NUM_ELEMENTS_INDICES];
	GLfloat colors[NUM_ELEMENTS_COLORS];
	GLfloat texture_uvs[NUM_ELEMENTS_TEXTURE_UVS];

	/* opengl buffers */
	GLuint vao;
	GLuint vbo_vertices;
	GLuint ebo_vertices;
	GLuint vbo_colors;
	GLuint vbo_texture_uvs;

	GLuint shader;
	int y;
	GLfloat key_width;
	int midi_note_number_lowest;

	piano_roll_note_t displayed_notes[MAX_NUM_DISPLAYED_NOTES];
	int note_counter;

	piano_roll_note_t *active_notes[NUM_MIDI_NOTE_NUMBERS];
} piano_roll_t;

void piano_roll_init(piano_roll_t *pr, texture_atlas_t *texture_atlas, GLuint shader, int y, GLfloat key_width, int midi_note_number_lowest);
void piano_roll_uninit(piano_roll_t *pr);

void piano_roll_gl_data_create(piano_roll_t *pr);
void piano_roll_gl_init(piano_roll_t *pr);
void piano_roll_gl_uninit(piano_roll_t *pr);

void piano_roll_receive_midi_note_on(piano_roll_t *pr, double time, u_char channel, u_char note, u_char velocity);
void piano_roll_receive_midi_note_off(piano_roll_t *pr, double time, u_char channel, u_char note, u_char velocity);

void piano_roll_update(piano_roll_t *pr, double run_time_ms);
void piano_roll_reset(piano_roll_t *pr, double run_time_ms);
void piano_roll_render(piano_roll_t *pr, mat4 *MVP);