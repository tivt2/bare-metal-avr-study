/* 1_blink
 * In this example, we configure one GPIO pin as an output to blink an LED */

#include <inttypes.h>

/* I/O registers in the ATmega328P are mapped starting at address 0x20.
 * To correctly select the I/O pin that we are going to modify/configure,
 * 2 macros are defined. One defines the I/O address offset, and the other
 * macro is responsible for correctly retrieving the address as a volitile
 * pointer derreferenced so we can store/read a values from it */

/* I/O register offset */
#define IO_REG_OFFSET 0x20

/* In order for the compiler let us modify the MCU registers
 * we must declare the register address as volatile */
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

int main(void) {
	/* The register DDRB(address 0x20 + 0x04) is responsible for setting the
	 * direction of data in the register I/O PORT B, where each bit of the
	 * register DDRB correspond to a PORT B bit and also corresponding PIN */

	/* In this case the notation (1 << 5) correspond to bit 5 in the PORT B
	 * register, which is being set to 1 by the OR operation '|=',
	 * setting PB5 as an OUTPUT in the MCU */
	IO_PIN(0x04) |= (1 << 5);

	while (1) {
		/* Here we target PORTB(addres 0x20 + 0x05) register and with the
		 * XOR operation '^=', we toggle the PB5 which correspond to DDRB bit 5
		 */
		IO_PIN(0x05) ^= (1 << 5);

		/* A simple NOP loop to simulate a delay */
		for (long int i = 2000000; i > 0; i--) {
		}
	}

	return 0;
}
