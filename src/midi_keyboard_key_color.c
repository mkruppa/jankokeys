#include "midi_keyboard_key_color.h"
#include "easing.h"

void midi_keyboard_key_color_rgb(float r, float g, float b, midi_keyboard_key_color_t dst_color)
{
	dst_color[ 0] = r;    dst_color[ 1] = g;    dst_color[ 2] = b;
	dst_color[ 3] = r;    dst_color[ 4] = g;    dst_color[ 5] = b;
	dst_color[ 6] = r;    dst_color[ 7] = g;    dst_color[ 8] = b;
	dst_color[ 9] = r;    dst_color[10] = g;    dst_color[11] = b;
}

void midi_keyboard_key_color_ease(double time, double duration, midi_keyboard_key_color_t color1, midi_keyboard_key_color_t color2, midi_keyboard_key_color_t dst_color)
{
	for (int i = 0; i < NUM_ELEMENTS_PER_COLOR; ++i) {
		dst_color[i] = easing_lerp(easing_smooth_stop4(time / duration), color1[i], color2[i]);
	}
}