#include "sdl_clock.h"

#define MILLISECONDS_IN_A_SECOND 1000

Uint64 sdl_clock_tick()
{
	return SDL_GetPerformanceCounter();
}

void sdl_clock_start(sdl_clock_t *clock)
{
	clock->tick_start = SDL_GetPerformanceCounter();
}

double sdl_clock_tick_delta_to_ms(Uint64 delta)
{
	return (double)(delta * MILLISECONDS_IN_A_SECOND) / SDL_GetPerformanceFrequency();
}

double sdl_clock_elapsed_ms(sdl_clock_t *clock)
{
	return sdl_clock_tick_delta_to_ms(SDL_GetPerformanceCounter() - clock->tick_start);
}

double sdl_clock_restart(sdl_clock_t *clock)
{
	Uint64 tick_now = SDL_GetPerformanceCounter();
	double elapsed_ms = sdl_clock_tick_delta_to_ms(tick_now - clock->tick_start);
	clock->tick_start = tick_now;
	return elapsed_ms;
}