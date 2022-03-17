#pragma once

#include <SDL_timer.h>

typedef struct sdl_clock {
	Uint64 tick_start;
} sdl_clock_t;

Uint64 sdl_clock_tick();
void sdl_clock_start(sdl_clock_t *clock);
double sdl_clock_tick_delta_to_ms(Uint64 delta);
double sdl_clock_elapsed_ms(sdl_clock_t *clock);
double sdl_clock_restart(sdl_clock_t *clock);