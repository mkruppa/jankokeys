#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

ssize_t file_load(const char *filename, char **contents)
{
	*contents = NULL;
	size_t contents_size = 0L;

	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: FILEIO: %s '%s'\n", strerror(errno), filename);
		abort();
	}
	ssize_t bytes_read = getdelim(contents, &contents_size, '\0', file);
	if (bytes_read == -1) {
		fprintf(stderr, "ERROR: FILEIO: %s\n", strerror(errno));
		abort();
	}
	fclose(file);
	return bytes_read;
}

GLuint shader_create_from_source(const char *shader_source, GLenum shader_type)
{
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	return shader;
}

void shader_destroy(GLuint shader)
{
	glDeleteShader(shader);
}

GLuint shaders_load_from_source(const char *vert_shader_source, const char *frag_shader_source)
{
	GLuint vert_shader = shader_create_from_source(vert_shader_source, GL_VERTEX_SHADER);
	GLuint frag_shader = shader_create_from_source(frag_shader_source, GL_FRAGMENT_SHADER);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	shader_destroy(vert_shader);
	shader_destroy(frag_shader);
	return shader_program;
}

GLuint shader_program_create_from_file(const char *vert_shader_filename, const char *frag_shader_filename)
{
	char *vert_shader_source = NULL;
	char *frag_shader_source = NULL;
	file_load(vert_shader_filename, &vert_shader_source);
	file_load(frag_shader_filename, &frag_shader_source);

	GLuint shader_program = shaders_load_from_source(vert_shader_source, frag_shader_source);

	free(vert_shader_source);
	free(frag_shader_source);
	return shader_program;
}

void shader_program_destroy(GLuint shader_program)
{
	glDeleteProgram(shader_program);
}