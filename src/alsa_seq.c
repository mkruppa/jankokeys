#include "alsa_seq.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MIDI_EVENT_NOTE_ON 0x90
#define MIDI_EVENT_NOTE_OFF 0x80

#define MIDI_MESSAGE_CONTROL_CHANGE 0xB0
#define MIDI_EVENT_SUSTAIN 0x40
#define MIDI_EVENT_SUSTAIN_ON 0x7F
#define MIDI_EVENT_SUSTAIN_OFF 0

midi_seq_t *midi_seq_client_open(const char *client_name)
{
	snd_seq_t *seq = NULL;
	snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
	snd_seq_set_client_name(seq, client_name);
	return seq;
}

void midi_seq_client_close(midi_seq_t *seq)
{
	snd_seq_close(seq);
}

int midi_seq_port_create(midi_seq_t *seq, const char *port_name)
{
	return snd_seq_create_simple_port(seq, port_name,
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE |
		SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_MIDI_GENERIC);
}

void midi_seq_port_destroy(midi_seq_t *seq, int port_id)
{
	snd_seq_delete_simple_port(seq, port_id);
}

void midi_seq_event_send_message(midi_seq_t *seq, int port_id, const unsigned char *msg, size_t len)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);

	snd_midi_event_t *dev;
	snd_midi_event_new(len, &dev);
	snd_midi_event_encode(dev, msg, len, &ev);
	snd_midi_event_free(dev);

	snd_seq_ev_set_source(&ev, port_id);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);

	snd_seq_event_output(seq, &ev);
	snd_seq_drain_output(seq);
}

void midi_seq_event_send_note_on(midi_seq_t *seq, int port_id, int channel, int key, int velocity)
{
	const unsigned char msg[] = {MIDI_EVENT_NOTE_ON + channel, key, velocity};
	midi_seq_event_send_message(seq, port_id, msg, ARRAY_SIZE(msg));
}

void midi_seq_event_send_note_off(midi_seq_t *seq, int port_id, int channel, int key, int velocity)
{
	const unsigned char msg[] = {MIDI_EVENT_NOTE_OFF + channel, key, velocity};
	midi_seq_event_send_message(seq, port_id, msg, ARRAY_SIZE(msg));
}

void midi_seq_event_send_sustain(midi_seq_t *seq, int port_id, int value)
{
	const unsigned char msg[] = {MIDI_MESSAGE_CONTROL_CHANGE, MIDI_EVENT_SUSTAIN, value};
	midi_seq_event_send_message(seq, port_id, msg, ARRAY_SIZE(msg));
}

void midi_seq_event_send_sustain_on(midi_seq_t *seq, int port_id)
{
	midi_seq_event_send_sustain(seq, port_id, MIDI_EVENT_SUSTAIN_ON);
}

void midi_seq_event_send_sustain_off(midi_seq_t *seq, int port_id)
{
	midi_seq_event_send_sustain(seq, port_id, MIDI_EVENT_SUSTAIN_OFF);
}