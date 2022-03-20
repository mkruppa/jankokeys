#pragma once

#include <GL/glew.h>

#define NUM_VERTICES_PER_KEY 4
#define NUM_COLOR_CHANNELS_PER_VERTEX 3
#define NUM_ELEMENTS_PER_COLOR (NUM_VERTICES_PER_KEY * NUM_COLOR_CHANNELS_PER_VERTEX)

typedef GLfloat midi_keyboard_key_color_t[NUM_ELEMENTS_PER_COLOR];

void midi_keyboard_key_color_rgb(float r, float g, float b, midi_keyboard_key_color_t dst_color);
void midi_keyboard_key_color_ease(double time, double duration, midi_keyboard_key_color_t color1, midi_keyboard_key_color_t color2, midi_keyboard_key_color_t dst_color);