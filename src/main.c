#include "window.h"

int main(void)
{
	window_t win;
	if (window_create(&win, "jankokeys")) { window_run(&win); }
	window_destroy(&win);
	return EXIT_SUCCESS;
}