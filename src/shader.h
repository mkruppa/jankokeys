#pragma once

#include <sys/types.h>
#include <GL/glew.h>

ssize_t load_file(const char *filename, char **contents);

GLuint create_shader_from_source(const char *shader_source, GLenum shader_type);

GLuint load_shaders_from_source(const char *vert_shader_source, const char *frag_shader_source);

GLuint load_shaders_from_file(const char *vert_shader_filename, const char *frag_shader_filename);