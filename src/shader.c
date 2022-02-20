#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

ssize_t load_file(const char *filename, char **contents)
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

GLuint create_shader_from_source(const char *shader_source, GLenum shader_type)
{
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	return shader;
}

GLuint load_shaders_from_source(const char *vert_shader_source, const char *frag_shader_source)
{
	GLuint vert_shader = create_shader_from_source(vert_shader_source, GL_VERTEX_SHADER);
	GLuint frag_shader = create_shader_from_source(frag_shader_source, GL_FRAGMENT_SHADER);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	return shader_program;
}

GLuint load_shaders_from_file(const char *vert_shader_filename, const char *frag_shader_filename)
{
	char *vert_shader_source = NULL;
	char *frag_shader_source = NULL;
	load_file(vert_shader_filename, &vert_shader_source);
	load_file(frag_shader_filename, &frag_shader_source);

	GLuint shader_program = load_shaders_from_source(vert_shader_source, frag_shader_source);

	free(vert_shader_source);
	free(frag_shader_source);
	return shader_program;
}
