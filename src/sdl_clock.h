#pragma once

#include <SDL_stdinc.h>

typedef Uint64 sdl_tick_t;

typedef struct sdl_clock {
	sdl_tick_t tick_start;
} sdl_clock_t;

sdl_tick_t sdl_clock_tick();

void sdl_clock_start(sdl_clock_t *clock);
double sdl_clock_tick_delta_to_ms(sdl_tick_t delta);
double sdl_clock_elapsed_ms(sdl_clock_t *clock);
double sdl_clock_restart(sdl_clock_t *clock);