#pragma once

struct window;
typedef struct window window_t;

void midi_process_events(window_t *win, double run_time_ms);