/* 2_button_polling */

#include "avr_atmega328p.h"

int main(void) {
	// set DDRB5 as OUTPUT
	SET_BIT(DDRB, 5);

	// set DDRB0 as INPUT
	UNSET_BIT(DDRB, 0);

	while (1) {
		/* In the ATmega328P archtecture there are a distinction in the
		 * registers of GPIO where the PORTB registers mainly represent writing
		 * to that pin data, and PINB represents reading that from that pin
		 * data, in this case if are reading the button pin so we can change the
		 * state of the LED, we must read from the PINB register (addres 0x03 +
		 * 0x20)*/
		if (READ_BIT(PINB, 0) != 0) {
			// set PORTB5 to HIGH
			SET_BIT(DDRB, 5);
		} else {
			// set PORTB5 to LOW
			UNSET_BIT(DDRB, 5);
		}
	}

	return 0;
}
