#pragma once
#include "alsa_seq.h"
#include "midi_keyboard_janko.h"

void midi_process_events(midi_seq_t *seq, midi_keyboard_janko_t *kb);