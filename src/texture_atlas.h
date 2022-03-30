#pragma once
#include <GL/gl.h>

typedef struct texture_atlas {
	int w;
	int h;
	int tw;
	int th;

	GLuint texture_id;
} texture_atlas_t;

#define NUM_VERTICES_PER_QUAD 4
#define NUM_DIMENSIONS_PER_UV 2
typedef GLfloat uv_quad_t[NUM_VERTICES_PER_QUAD * NUM_DIMENSIONS_PER_UV];

void texture_atlas_create(texture_atlas_t *texture_atlas, const char* filename);
void texture_atlas_destroy(texture_atlas_t *texture_atlas);

void texture_atlas_set_tile_size(texture_atlas_t *texture_atlas, int tw, int th);
void texture_atlas_get_uv_quad(texture_atlas_t *texture_atlas, int x, int y, uv_quad_t dst_uv);