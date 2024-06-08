#include <inttypes.h>

/* I/O registers in the AVR architecture starts at address 0x20
 * in order to select correctly the I/O pin that we are going to
 * modify/configure 2 macros are defined, one defines the I/O address offset
 * and the other macro is responsible for correctly retrieving the address
 * as a volitile pointer derreferenced */

/* I/O register offset */
#define IO_REG_OFFSET 0x20

/* in order for the compiler let us modify the MCU registers
 * we must declare the register address as volatile */
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

int main(void) {
	/* the register DDRB(address 0x20 + 0x04) is responsible for setting the
	 * direction of data in the register I/O PORT B, where each bit of the
	 * register DDRB correspond to a PORT PIN */

	/* in this case the notation (1 << 5) correspond to PIN 5 in the PORT B
	 * register that is being set to 1, corresponding to OUTPUT setting */
	IO_PIN(0x04) |= (1 << 5);

	while (1) {
		/* here we toggle PORT B(addres 0x20 + 0x05) with the XOR operation
		 * and targeting the PIN 5 correspondent to DDRB bit 5 */
		IO_PIN(0x05) ^= (1 << 5);

		/* just a simple NOP loop to simulate a delay */
		for (long int i = 2000000; i > 0; i--) {
		}
	}
}
