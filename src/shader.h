#pragma once

#include <sys/types.h>
#include <GL/glew.h>

ssize_t file_load(const char *filename, char **contents);

GLuint shader_create_from_source(const char *shader_source, GLenum shader_type);
void shader_destroy(GLuint shader);

GLuint shaders_load_from_source(const char *vert_shader_source, const char *frag_shader_source);

GLuint shader_program_create_from_file(const char *vert_shader_filename, const char *frag_shader_filename);
void shader_program_destroy(GLuint shader_program);