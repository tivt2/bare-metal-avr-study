/* 3_interrupt */

#include "avr_atmega328p.h"

/* Here we define a interrupt handler using the macro ISR(vector_idx)
 * more on that at, "avr_atmega328p.h" */
ISR(INT0_VEC) {
	for (int i = 0; i < 10; i++) {
		// Toggle the LED
		TOGGLE_BIT(PORTD, 7);
		// Fake delay of 100000 iterations
		for (volatile long int j = 100000; j > 0; j--) {
		}
	}
}

int main(void) {
	/* The EIMSK register is the External Interrupt Mask and it is resposible
	 * for enabling or disabling external interrupts, in this case we want to
	 * trigger the interrupt only when the button is pressed that is connected
	 * to the pin correspondent to INT0, here the bit 0 of the register
	 * correspond to enabling the INT0 external interrupt */
	SET_BIT(EIMSK, 0);

	/* The EICRA register is the External Interrupt Control Register A, its
	 * responsible to configure the external interrupt behaviour of INT0 and
	 * INT1, here we are setting its bit 1 (flag ISC01) to HIGH, which configure
	 * the interrupt to trigger when the external signal of INT0 goes from HIGH
	 * to LOW (falling edge) */
	SET_BIT(EICRA, 1);

	/* The SREG register is the AVR Status Register, where we can find the
	 * global interrupt flag at bit 7, in order to trigger any interrupt we need
	 * to enable this flag. To avoid triggering an interrupt before the INT0 pin
	 * is correctly settup we set this flag as last */
	SET_BIT(SREG, 7);

	// Make the PD7 be an OUTPUT pin (LED pin)
	SET_BIT(DDRD, 7);

	while (1) {
		// Toggle the LED
		TOGGLE_BIT(PORTD, 7);
		// Fake delay of 500000 iterations
		for (volatile long int i = 500000; i > 0; i--) {
		}
	}

	return 0;
}
