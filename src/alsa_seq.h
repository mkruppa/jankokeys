#pragma once

#include <alsa/asoundlib.h>

#define MIDI_THROUGH_CLIENT_ID 14
#define MIDI_THROUGH_PORT_ID 0

#define midi_seq_connect_to snd_seq_connect_to

typedef snd_seq_t midi_seq_t;

midi_seq_t *midi_seq_client_open(const char *client_name);

void midi_seq_client_close(midi_seq_t *seq);

int midi_seq_port_create(midi_seq_t *seq, const char *port_name);

void midi_seq_port_destroy(midi_seq_t *seq, int port_id);

void midi_seq_event_send_message(midi_seq_t *seq, int port_id, const unsigned char *msg, size_t len);

void midi_seq_event_send_note_on(midi_seq_t *seq, int port_id, int channel, int key, int velocity);

void midi_seq_event_send_note_off(midi_seq_t *seq, int port_id, int channel, int key, int velocity);

void midi_seq_event_send_sustain(midi_seq_t *seq, int port_id, int value);

void midi_seq_event_send_sustain_on(midi_seq_t *seq, int port_id);

void midi_seq_event_send_sustain_off(midi_seq_t *seq, int port_id);