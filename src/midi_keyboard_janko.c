#include "midi_keyboard_janko.h"

#include "shader.h"

static GLfloat data[] = {
	-1.0F, -1.0F,  0.0F,
	 1.0F, -1.0F,  0.0F,
	 0.0F,  1.0F,  0.0F
};

void midi_keyboard_janko_init(midi_keyboard_janko_t *kb, GLuint shader)
{
	glGenVertexArrays(1, &kb->vao);
	glBindVertexArray(kb->vao);

	glGenBuffers(1, &kb->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, kb->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	GLint pos = glGetAttribLocation(shader, "vert_pos");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void midi_keyboard_janko_render(midi_keyboard_janko_t *kb)
{
	glBindVertexArray(kb->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}