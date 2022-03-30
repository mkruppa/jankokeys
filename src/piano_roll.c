#include <GL/glew.h>
#include "piano_roll.h"

#include "midi_keyboard_key_color.h"
#include "sdl_clock.h"

#define DURATION_UNKNOWN -1
#define UNIT_PIXELS_PER_SECOND 400

static midi_keyboard_key_color_t color;
static uv_quad_t uv_note;

void piano_roll_init(piano_roll_t *pr, texture_atlas_t *texture_atlas, GLuint shader, int y, GLfloat key_width, int midi_note_number_lowest)
{
	*pr = (piano_roll_t){
		.shader = shader,
		.y = y,
		.key_width = key_width,
		.midi_note_number_lowest = midi_note_number_lowest,
	};

	midi_keyboard_key_color_rgb(0.2F, 0.2F, 0.9F, color);

	texture_atlas_get_uv_quad(texture_atlas, 5, 0, uv_note);

	piano_roll_gl_data_create(pr);
	piano_roll_gl_init(pr);
}

void piano_roll_uninit(piano_roll_t *pr)
{
	piano_roll_gl_uninit(pr);
}

static void piano_roll_gl_data_create_vertex(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat *vertex)
{
	vertex[ 0] = x1;        vertex[ 1] = y1;        vertex[ 2] = 0.0F;
	vertex[ 3] = x2;        vertex[ 4] = y1;        vertex[ 5] = 0.0F;
	vertex[ 6] = x1;        vertex[ 7] = y2;        vertex[ 8] = 0.0F;
	vertex[ 9] = x2;        vertex[10] = y2;        vertex[11] = 0.0F;
}

static void piano_roll_gl_data_create_index(size_t i, GLuint *index)
{
	GLuint j = i * NUM_VERTICES_PER_KEY;
	index[0] = j + 0;        index[1] = j + 1;        index[2] = j + 2;
	index[3] = j + 2;        index[4] = j + 1;        index[5] = j + 3;
}

void piano_roll_gl_data_create(piano_roll_t *pr)
{
	GLfloat vertex[NUM_ELEMENTS_PER_VERTEX];
	GLuint index[NUM_ELEMENTS_PER_INDEX];

	for (size_t i = 0; i < MAX_NUM_DISPLAYED_NOTES; ++i) {
		piano_roll_gl_data_create_vertex(0.0F, 0.0F, 0.0F, 0.0F, vertex);
		piano_roll_gl_data_create_index(i, index);

		memcpy(&pr->vertices[i * NUM_ELEMENTS_PER_VERTEX], vertex, SIZE_VERTEX);
		memcpy(&pr->indices[i * NUM_ELEMENTS_PER_INDEX], index, SIZE_INDEX);
		memcpy(&pr->colors[i * NUM_ELEMENTS_PER_COLOR], color, SIZE_COLOR);
		memcpy(&pr->texture_uvs[i * NUM_ELEMENTS_PER_TEXTURE_UV], uv_note, SIZE_TEXTURE_UV);
	}
}

void piano_roll_gl_init(piano_roll_t *pr)
{
	glGenVertexArrays(1, &pr->vao);
	glBindVertexArray(pr->vao);

	/* vertices */
	glGenBuffers(1, &pr->vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, pr->vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, MAX_NUM_DISPLAYED_NOTES * SIZE_VERTEX, pr->vertices, GL_DYNAMIC_DRAW);

	GLint pos = glGetAttribLocation(pr->shader, "vert_pos");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, NUM_DIMENSIONS_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

	/* indices */
	glGenBuffers(1, &pr->ebo_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pr->ebo_vertices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_NUM_DISPLAYED_NOTES * SIZE_INDEX, pr->indices, GL_STATIC_DRAW);

	/* colors */
	glGenBuffers(1, &pr->vbo_colors);
	glBindBuffer(GL_ARRAY_BUFFER, pr->vbo_colors);
	glBufferData(GL_ARRAY_BUFFER, MAX_NUM_DISPLAYED_NOTES * SIZE_COLOR, pr->colors, GL_DYNAMIC_DRAW);

	GLint col = glGetAttribLocation(pr->shader, "vert_col");
	glEnableVertexAttribArray(col);
	glVertexAttribPointer(col, NUM_COLOR_CHANNELS_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

	/* texture uvs */
	glGenBuffers(1, &pr->vbo_texture_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, pr->vbo_texture_uvs);
	glBufferData(GL_ARRAY_BUFFER, MAX_NUM_DISPLAYED_NOTES * SIZE_TEXTURE_UV, pr->texture_uvs, GL_STATIC_DRAW);

	GLint tex = glGetAttribLocation(pr->shader, "vert_tex");
	glEnableVertexAttribArray(tex);
	glVertexAttribPointer(tex, NUM_DIMENSIONS_PER_UV, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void piano_roll_gl_uninit(piano_roll_t *pr)
{
	glDeleteVertexArrays(1, &pr->vao);
	glDeleteBuffers(1, &pr->vbo_vertices);
	glDeleteBuffers(1, &pr->ebo_vertices);
	glDeleteBuffers(1, &pr->vbo_colors);
	glDeleteBuffers(1, &pr->vbo_texture_uvs);
}

void piano_roll_gl_upload_n_vertices(piano_roll_t *pr, GLintptr offset, GLsizeiptr n, const GLvoid * data)
{
	glBindVertexArray(pr->vao);
	glBindBuffer(GL_ARRAY_BUFFER, pr->vbo_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, n * SIZE_VERTEX, data);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void piano_roll_receive_midi_note_on(piano_roll_t *pr, double time, u_char channel, u_char note, u_char velocity)
{
	int i = pr->note_counter % MAX_NUM_DISPLAYED_NOTES;
	pr->displayed_notes[i] = (piano_roll_note_t){
		.time = time,
		.duration = DURATION_UNKNOWN,

		.channel = channel,
		.note = note,
		.velocity = velocity,
	};
	++pr->note_counter;

	piano_roll_note_t *pr_note = pr->active_notes[note];
	if (pr_note != NULL) { pr_note->duration = time - pr_note->time; }
	pr->active_notes[note] = &pr->displayed_notes[i];
}


void piano_roll_receive_midi_note_off(piano_roll_t *pr, double time, u_char channel, u_char note, u_char velocity)
{
	(void)channel;
	(void)velocity;

	piano_roll_note_t *pr_note = pr->active_notes[note];
	if (pr_note != NULL) {
		pr_note->duration = time - pr_note->time;
		pr->active_notes[note] = NULL;
	}
}

void piano_roll_update(piano_roll_t *pr, double run_time_ms)
{
	for (int i = 0; i < MAX_NUM_DISPLAYED_NOTES; ++i) {
		int midi_key_id = pr->displayed_notes[i].note - pr->midi_note_number_lowest;
		div_t res = div(midi_key_id, 2);
		GLfloat x1 = res.quot * pr->key_width + res.rem * (pr->key_width / 2);
		GLfloat x2 = x1 + pr->key_width;

		// y2(t) = (t - t0) m + b
		GLfloat y2 = (run_time_ms - pr->displayed_notes[i].time) / MILLISECONDS_IN_A_SECOND * UNIT_PIXELS_PER_SECOND + pr->y;

		GLfloat y1 = pr->displayed_notes[i].duration == DURATION_UNKNOWN ? pr->y :
			y2 - pr->displayed_notes[i].duration / MILLISECONDS_IN_A_SECOND * UNIT_PIXELS_PER_SECOND;

		GLfloat vertex[NUM_ELEMENTS_PER_VERTEX];
		piano_roll_gl_data_create_vertex(x1, x2, y2, y1, vertex);
		memcpy(&pr->vertices[i * NUM_ELEMENTS_PER_VERTEX], vertex, SIZE_VERTEX);
	}
	piano_roll_gl_upload_n_vertices(pr, 0, MAX_NUM_DISPLAYED_NOTES, pr->vertices);
}

void piano_roll_reset(piano_roll_t *pr, double run_time_ms)
{
	// pr->note_counter = 0;
	// for (int i = 0; i < MAX_NUM_DISPLAYED_NOTES; ++i) { pr->displayed_notes[i] = (piano_roll_note_t){0}; }

	for (int i = 0; i < NUM_MIDI_NOTE_NUMBERS; ++i) {
		piano_roll_note_t *pr_note = pr->active_notes[i];
		if (pr_note != NULL) { pr_note->duration = run_time_ms - pr_note->time; }
		pr->active_notes[i] = NULL;
	}
}

void piano_roll_render(piano_roll_t *pr, mat4 *MVP)
{
	glBindVertexArray(pr->vao);
	glUniformMatrix4fv(glGetUniformLocation(pr->shader, "M"), 1, GL_FALSE, (GLfloat*)MVP);
	glDrawElements(GL_TRIANGLES, MAX_NUM_DISPLAYED_NOTES * NUM_ELEMENTS_PER_INDEX, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}