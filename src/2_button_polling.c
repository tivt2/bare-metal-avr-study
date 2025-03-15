/* 2_button_polling */

#include "avr_atmega328p.h"

int main(void) {
	// set PD7 as OUTPUT (LED pin)
	SET_BIT(DDRD, 7);

	// set PD2 as INPUT (push button pin)
	UNSET_BIT(DDRD, 2);

	/* Im setting the code button_state as 0(LOW), even tho the default state is
	 * 1(HIGH) only so that the LED starts turned off */
	uint8_t button_state = 0;
	while (1) {

		uint8_t last_button_state = button_state;
		/* Reading PD2 pin every loop cycle means that
		 * we are working with GPIO polling */
		button_state = READ_BIT(PIND, 2);

		/* Since the ATmega328P uses by default pull up resistors when the pin
		 * is set as INPUT, the value read from PIND2 will be by default 1(HIGH)
		 * this means that the button is only at a 'pressed' state when PIND2
		 * reads 0(LOW) */
		if (button_state != last_button_state && button_state == 0) {
			TOGGLE_BIT(PORTD, 7);
		}
	}

	return 0;
}
