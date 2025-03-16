/* 4_timers */

#include "avr_atmega328p.h"
#include <stdint.h>

int main(void) {
	// Set OC1A (PB1) as OUTPUT
	SET_BIT(DDRB, 1);

	// Setting the flag COM1A0 will toggle the OC1A (PB1) pin on compare match
	SET_BIT(TCCR1A, 6);

	/* Setting the flags CS12 will configure the prescaller to
	 * increment the timer every 256 ticks of the input clock */
	SET_BIT(TCCR1B, 2);

	/* Since we want to achieve a toggle of the OC1A (PB1) every second,
	 * we must setup the compare register to a value of 62500, this is because
	 * if we take the CPU clock input (16000000hz) and the prescaler, we get
	 * 16000000 / 256 = 62500hz, this means we need 62500 ticks to reach 1
	 * second.
	 * To make this work we calculate the compare value (-1 since the count
	 * starts at 0) and write it to the OCR1AH and OCR1AL registers with the
	 * following logic */
	uint16_t compare_value = (16000000 / 256) - 1;
	// Take 8 Most significant bits and write it to the uint8_t address
	GET_ADDR(OCR1AH) = (compare_value >> 8);
	// Take 8 Least significant bits and write it to the uint8_t address
	GET_ADDR(OCR1AL) = (compare_value & 0xFF);

	/* Configure the mode of operation to CTC (Clear Timer on Compare match),
	 * this will toggle OC1A when matching the timer with OCR1A and clear the
	 * timer */
	SET_BIT(TCCR1B, 3);

	while (1) {
		// infinite loop
	}
}
