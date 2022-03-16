#include "midi_process_events.h"

#define NUM_MIDI_KEYBOARD_KEYS 88

typedef enum channel_mode_message {
	CHANNEL_MODE_MESSAGE_ALL_SOUND_OFF = 0x78,
	CHANNEL_MODE_MESSAGE_RESET_ALL_CONTROLLERS,
	CHANNEL_MODE_MESSAGE_LOCAL_CONTROL,
	CHANNEL_MODE_MESSAGE_ALL_NOTES_OFF
} channel_mode_message_t;

void midi_process_events(midi_seq_t *seq, midi_keyboard_janko_t *kb)
{
	snd_seq_event_t *ev;
	while (snd_seq_event_input(seq, &ev) >= 0) {
		switch (ev->type) {
		case SND_SEQ_EVENT_NOTEON:
			printf("INFO: ALSA SEQ: EVENT RECEIVED: %s [channel: %d | note: %d | velocity: %d]\n", "SND_SEQ_EVENT_NOTEON",
				ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
			if (ev->data.note.velocity == 0) {
				midi_keyboard_janko_receive_midi_note_off(kb, ev->data.note.note);
			} else {
				midi_keyboard_janko_receive_midi_note_on(kb, ev->data.note.note);
			}
			break;
		case SND_SEQ_EVENT_NOTEOFF:
			printf("INFO: ALSA SEQ: EVENT RECEIVED: %s [channel: %d | note: %d]\n", "SND_SEQ_EVENT_NOTEOFF",
				ev->data.note.channel, ev->data.note.note);
			midi_keyboard_janko_receive_midi_note_off(kb, ev->data.note.note);
			break;
		case SND_SEQ_EVENT_CONTROLLER:
			printf("INFO: ALSA SEQ: EVENT RECEIVED: %s [channel: %d | param: %d | value: %d]\n", "SND_SEQ_EVENT_CONTROLLER",
				ev->data.control.channel, ev->data.control.param, ev->data.control.value);
			if (ev->data.control.param == CHANNEL_MODE_MESSAGE_ALL_NOTES_OFF) {
				for (size_t key_id = 0; key_id < NUM_MIDI_KEYBOARD_KEYS; ++key_id) {
					kb->keys_pressed[key_id] = 0;
				}
			}
			break;
		default:
			printf("INFO: ALSA SEQ: EVENT RECEIVED: type: %d\n", ev->type);
		}
	}
}