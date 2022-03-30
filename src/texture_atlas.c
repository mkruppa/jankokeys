#include "texture_atlas.h"
#include <SDL_image.h>

void texture_atlas_create(texture_atlas_t *texture_atlas, const char* filename)
{
	SDL_Surface *img = IMG_Load(filename);
	if (img == NULL) {
		fprintf(stderr, "ERROR: SDL_IMAGE: %s", IMG_GetError());
		abort();
	}

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);

	texture_atlas->w = img->w;
	texture_atlas->h = img->h;
	texture_atlas->texture_id = texture_id;

	free(img);
}

void texture_atlas_destroy(texture_atlas_t *texture_atlas)
{
	glDeleteTextures(1, &texture_atlas->texture_id);
}

void texture_atlas_set_tile_size(texture_atlas_t *texture_atlas, int tw, int th)
{
	texture_atlas->tw = tw;
	texture_atlas->th = th;
}

void texture_atlas_get_uv_quad(texture_atlas_t *texture_atlas, int x, int y, uv_quad_t dst_uv)
{
	GLfloat normalized_tw = (GLfloat)texture_atlas->tw / texture_atlas->w;
	GLfloat normalized_th = (GLfloat)texture_atlas->th / texture_atlas->h;

	GLfloat x1 = x * normalized_tw;
	GLfloat x2 = x1 + normalized_tw;
	GLfloat y1 = y * normalized_th;
	GLfloat y2 = y1 + normalized_th;

	uv_quad_t uv = {
		x1, 	y2,
		x2, 	y2,
		x1, 	y1,
		x2, 	y1
	};
	memcpy(dst_uv, uv, sizeof(uv_quad_t));
}