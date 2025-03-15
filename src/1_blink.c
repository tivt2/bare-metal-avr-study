/* 1_blink */

#include "avr_atmega328p.h"

int main(void) {
	/* Set the DDB5 to 1, making the PB5 be an output pin */
	SET_BIT(DDRB, 5);

	while (1) {
		/* Toggle PORTB5, since the built-in LED is constantly receiving the
		 * data of PB5 after we set it as output it will make the LED light up
		 * when it is set to 1 and light off when the bit is set to 0, making
		 * the LED blink */
		TOGGLE_BIT(PORTB, 5);

		/* A simple NOP loop to simulate a delay so we can visually see the LED
		 * blinking, we declare the 'i' variable as volitile just so the
		 * compiler doesn't optimize and remove the loop from the program */
		for (volatile long int i = 500000; i > 0; i--) {
		}
	}

	return 0;
}
