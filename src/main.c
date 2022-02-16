#include <stdio.h>
#include "alsa_seq.h"

#define MIDI_THROUGH_CLIENT_ID 14
#define MIDI_THROUGH_PORT_ID 0

#define MIDI_NOTE_MIDDLE_C 60

int main(void)
{
	/* initialize alsa midi sequencer */
	midi_seq_t *seq = midi_seq_client_open("jankokeys");
	int port_id = midi_seq_port_create(seq, "midi_port");

	/* connect to the midi through port */
	midi_seq_connect_to(seq, port_id, MIDI_THROUGH_CLIENT_ID, MIDI_THROUGH_PORT_ID);

	/* send sample midi events */
	midi_seq_event_send_note_on(seq, port_id, 0, MIDI_NOTE_MIDDLE_C, 100);
	sleep(1);
	midi_seq_event_send_note_off(seq, port_id, 0, MIDI_NOTE_MIDDLE_C, 100);

	sleep(1);

	midi_seq_event_send_sustain_on(seq, port_id);
	midi_seq_event_send_note_on(seq, port_id, 0, MIDI_NOTE_MIDDLE_C, 100);
	sleep(1);
	midi_seq_event_send_note_off(seq, port_id, 0, MIDI_NOTE_MIDDLE_C, 100);
	sleep(5);
	midi_seq_event_send_sustain_off(seq, port_id);

	getchar();

	/* clean up */
	midi_seq_port_destroy(seq, port_id);
	midi_seq_client_close(seq);
	return EXIT_SUCCESS;
}