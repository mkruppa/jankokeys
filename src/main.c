// #include <stdio.h>
// #include <stdlib.h>

// #include "alsa_seq.h"
// #include "config.h"
#include "window.h"

int main(void)
{
	// /* initialize alsa midi sequencer */
	// midi_seq_t *seq = midi_seq_client_open("jankokeys");
	// int port_id = midi_seq_port_create(seq, "midi_port");
	// /* connect to the midi through port */
	// midi_seq_connect_to(seq, port_id, MIDI_THROUGH_CLIENT_ID, MIDI_THROUGH_PORT_ID);

	// midi_seq_event_send_note_on(seq, port_id, 0, MIDI_NOTE_MIDDLE_C, 100);
	// midi_seq_event_send_note_off(seq, port_id, 0, MIDI_NOTE_MIDDLE_C, 100);

	// midi_seq_port_destroy(seq, port_id);
	// midi_seq_client_close(seq);


	// int keybinding[NUM_SCANCODES] = {0};
	// if (!config_load(keybinding)) {
	// 	exit(EXIT_FAILURE);
	// }

	// for (int i = 0; i < NUM_SCANCODES; ++i) {
	// 	if(keybinding[i] != 0) {
	// 		printf("%d: %d\n", i, keybinding[i]);
	// 	}
	// }


	window_t win;
	if (window_create(&win, "jankokeys", 600, 400)) { window_run(&win); }
	window_destroy(&win);
	
	return EXIT_SUCCESS;
}