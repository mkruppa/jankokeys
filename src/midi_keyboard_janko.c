#include "midi_keyboard_janko.h"

#include <GL/glew.h>

#include "shader.h"

static GLuint shader;

static GLuint vao;
static GLuint vbo;
static GLfloat data[] = {
	-1.0F, -1.0F,  0.0F,
	 1.0F, -1.0F,  0.0F,
	 0.0F,  1.0F,  0.0F
};

void midi_keyboard_janko_init(void)
{
	shader = load_shaders_from_file("../shader/simple.vert", "../shader/simple.frag");
	glUseProgram(shader);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	GLint pos = glGetAttribLocation(shader, "vert_pos");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void midi_keyboard_janko_render(void)
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}