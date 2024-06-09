/* 2_button_polling
 * In this example, we configure one GPIO pin as input so we can poll(read)
 * its value in terms to determine a button press*/

#include <inttypes.h>

#define IO_REG_OFFSET 0x20
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

int main(void) {
	// set DDRB5 as OUTPUT
	IO_PIN(0x04) |= (1 << 5);

	// set DDRB0 as INPUT
	IO_PIN(0x04) &= ~(1 << 0);

	while (1) {
		/* In the ATmega328P archtecture there are a distinction in the
		 * registers of GPIO where the PORTB registers mainly represent writing
		 * to that pin data, and PINB represents reading that from that pin
		 * data, in this case if are reading the button pin so we can change the
		 * state of the LED, we must read from the PINB register (addres 0x03 +
		 * 0x20)*/
		if ((IO_PIN(0x03) & (1 << 0)) != 0) {
			// set PORTB5 to HIGH
			IO_PIN(0x05) |= (1 << 5);
		} else {
			// set PORTB5 to LOW
			IO_PIN(0x05) &= ~(1 << 5);
		}
	}

	return 0;
}
