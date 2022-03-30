#include "midi_process_events.h"
#include "window.h"

typedef enum channel_mode_message {
	CHANNEL_MODE_MESSAGE_ALL_SOUND_OFF = 0x78,
	CHANNEL_MODE_MESSAGE_RESET_ALL_CONTROLLERS,
	CHANNEL_MODE_MESSAGE_LOCAL_CONTROL,
	CHANNEL_MODE_MESSAGE_ALL_NOTES_OFF
} channel_mode_message_t;

void midi_process_events_log_event_note_on(snd_seq_event_t *ev)
{
	printf("INFO: ALSA SEQ: EVENT RECEIVED: %s [channel: %d | note: %d | velocity: %d]\n", "SND_SEQ_EVENT_NOTEON",
		ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
}

void midi_process_events_log_event_note_off(snd_seq_event_t *ev)
{
	printf("INFO: ALSA SEQ: EVENT RECEIVED: %s [channel: %d | note: %d]\n", "SND_SEQ_EVENT_NOTEOFF",
		ev->data.note.channel, ev->data.note.note);
	// printf("INFO: ALSA SEQ: EVENT RECEIVED: %s [channel: %d | note: %d | velocity: %d | off_velocity: %d | duration: %d]\n", "SND_SEQ_EVENT_NOTEOFF",
	// 	ev->data.note.channel, ev->data.note.note, ev->data.note.velocity, ev->data.note.off_velocity, ev->data.note.duration);
}

void midi_process_events_log_event_controller(snd_seq_event_t *ev)
{
	printf("INFO: ALSA SEQ: EVENT RECEIVED: %s [channel: %d | param: %d | value: %d]\n", "SND_SEQ_EVENT_CONTROLLER",
		ev->data.control.channel, ev->data.control.param, ev->data.control.value);
}

void midi_process_events(window_t *win, double run_time_ms)
{
	snd_seq_event_t *ev;
	while (snd_seq_event_input(win->seq, &ev) >= 0) {
		switch (ev->type) {
		case SND_SEQ_EVENT_NOTEON:
			// midi_process_events_log_event_note_on(ev);
			if (ev->data.note.velocity == 0) {
				midi_keyboard_janko_receive_midi_note_off(&win->janko_keyboard, ev->data.note.note);
				piano_roll_receive_midi_note_off(&win->piano_roll, run_time_ms, ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
			} else {
				midi_keyboard_janko_receive_midi_note_on(&win->janko_keyboard, ev->data.note.note);
				piano_roll_receive_midi_note_on(&win->piano_roll, run_time_ms, ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
			}
			break;
		case SND_SEQ_EVENT_NOTEOFF:
			// midi_process_events_log_event_note_off(ev);
			midi_keyboard_janko_receive_midi_note_off(&win->janko_keyboard, ev->data.note.note);
			piano_roll_receive_midi_note_off(&win->piano_roll, run_time_ms, ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
			break;
		case SND_SEQ_EVENT_CONTROLLER:
			// midi_process_events_log_event_controller(ev);
			if (ev->data.control.param == CHANNEL_MODE_MESSAGE_ALL_NOTES_OFF) {
				midi_keyboard_janko_reset_keys(&win->janko_keyboard);
				piano_roll_reset(&win->piano_roll, run_time_ms);
			}
			break;
		default:
			printf("INFO: ALSA SEQ: EVENT RECEIVED: type: %d\n", ev->type);
		}
	}
}