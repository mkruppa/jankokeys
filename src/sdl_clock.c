#include "sdl_clock.h"
#include <SDL_timer.h>

sdl_tick_t sdl_clock_tick()
{
	return SDL_GetPerformanceCounter();
}

double sdl_clock_tick_to_ms(sdl_tick_t tick)
{
	return (double)(tick * MILLISECONDS_IN_A_SECOND) / SDL_GetPerformanceFrequency();
}

void sdl_clock_start(sdl_clock_t *clock)
{
	clock->tick_start = SDL_GetPerformanceCounter();
}

double sdl_clock_restart(sdl_clock_t *clock)
{
	sdl_tick_t tick_now = SDL_GetPerformanceCounter();
	double elapsed_ms = sdl_clock_tick_to_ms(tick_now - clock->tick_start);
	clock->tick_start = tick_now;
	return elapsed_ms;
}

sdl_tick_t sdl_clock_elapsed(sdl_clock_t *clock)
{
	return SDL_GetPerformanceCounter() - clock->tick_start;
}

double sdl_clock_elapsed_ms(sdl_clock_t *clock)
{
	return sdl_clock_tick_to_ms(sdl_clock_elapsed(clock));
}