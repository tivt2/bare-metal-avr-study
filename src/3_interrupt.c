/* 3_interrupt
 * In this example, we define and assign an Interrupt Service Routine(ISR) to
 * the interrupt vector table, the program consists into toggling a LED and
 * looping with NOOP a lot of cycles, the interrupt routine will be called when
 * a button is pressed and it will toggle the same LED much faster */

#include <inttypes.h>

#define IO_REG_OFFSET 0x20
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

#define PINB IO_PIN(0x03)
#define DDRB IO_PIN(0x04)
#define PORTB IO_PIN(0x05)

/* Here we copy this macro defined in the AVR-libc <avr/interrupt.h> so we can
 * add interrupt routines to the interrupt vector table */
#define INTERRUPT_ROUTINE(vector)                                              \
	void __vector_##vector(void)                                               \
		__attribute((signal, used, externally_visible));                       \
	void __vector_##vector(void)

/* Here we are defining the Interrupt routine that will be executed when we
 * trigger at the External interrupt 0, this call will define a function called
 * __vector_1 where 1 is the expected position in the vector table by the MCU to
 * store the INT0 interrupt routine, that will be correctly mapped in the vector
 * table by the compiler because of the macro definition */
INTERRUPT_ROUTINE(1) {
	for (int i = 0; i < 10; i++) {
		PORTB ^= (1 << 5);
		for (volatile long int j = 100000; j > 0; j--) {
		}
	}
}

int main(void) {
	/* The register EIMSK (address 0x1D + 0x20) is the External Interrupt Mask
	 * and it is resposible for enabling or disabling external interrupts, in
	 * this case we want to trigger the interrupt only when the button is
	 * pressed that is connected to the pin correspondent to INT0, here the bit
	 * 0 of the register correspond to enabling the INT0 external interrupt */
	IO_PIN(0x1D) |= (1 << 0);

	/* This register EICRA (address 0x49 + 0x20) is the External Interrupt
	 * Control Register A, its responsible to configure the external interrupt
	 * behaviour, here we are setting its bit 1 (flag ISC01) to HIGH, which will
	 * trigger the interrupt signal when the INT0 goes from HIGH to LOW */
	IO_PIN(0x49) |= (1 << 1);

	/* The register SREG (address 0x3F + 0x20) is the AVR Status Register, where
	 * we can find the global interrupt flag at bit 7, in order to trigger an
	 * interrupt we need to enable this flag */
	IO_PIN(0x3F) |= (1 << 7);

	/* basicaly from here on its the same as the blink example */
	DDRB |= (1 << 5);

	while (1) {
		PORTB ^= (1 << 5);
		for (volatile long int i = 500000; i > 0; i--) {
		}
	}

	return 0;
}
