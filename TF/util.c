#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "util.h"

int verbose_mode = 0;

void get_initial_flags(int argc, char** argv) {
	int value = 0;
	// Loop to look for flags
	while( (value = getopt(argc, argv, "v")) != -1 ) {
		switch( value ) {
			case 'v':
				verbose_mode = 1;
				break;
			case '?':
				break;
			default:
				break;
		}
	}
}

void d_printf(char* format, ...) {
	va_list arguments;
	if( !verbose_mode ) {
		return;
	}
	va_start(arguments, format);
	vprintf(format, arguments);
	fflush(stdin);
	va_end(arguments);
}
