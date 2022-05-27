#include "lines.h"

void lines_init() {
	// init lines pins (PORTB0 ... PORTB4)
	DDRB |= 0x1F;
	PORTB &= ~0x1F;
}

void lines_set(uint8_t ind, bool value) {
	if (value) {
		PORTB |= (1 << ind);
	} else {
		PORTB &= ~(1 << ind);
	}
}