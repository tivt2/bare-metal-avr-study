/* 4_timers
 * In this example, we utilize the 16-bit timer/counter peripheral in the
 * ATmega328P in order to toggle a specific pin and blink an LED, so in this
 * case we will not have a loop, and relly only into the timer */

#include <inttypes.h>

#define IO_REG_OFFSET 0x20
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

#define DDRB IO_PIN(0x04)

int main(void) {
	/* setting the data direction of OC1A(PINB1) correspondent pin to HIGH, this
	 * defines it as an OUTPUT, we connect the LED to this pin */
	DDRB |= (1 << 1);

	/* TCCR1A (address 0x60 + 0x20) Timer/Counter1 Control Register A, here we
	 * are setting the flag COM1A0(1 << 6) to HIGH in order to configure this
	 * timer as Compare Output Mode, non-PWM Mode, this mean that the pin
	 * correspondent to OC1A will be toggled everytime the timer is equal to the
	 * defined compare value(TOP) */
	IO_PIN(0x60) = (1 << 6);

	/* TCCR1B (address 0x61 + 0x20) Timer/Counter1 Control Register B, here we
	 * are setting the flags CS12(1 << 2) and CS10(1 << 0) to HIGH in order to
	 * configure the prescaler that will prescale the CPU clock and "slow down"
	 * the timer counting process, here we are making the counter count up every
	 * 16000000/1024 Hz, also we set WGM12(1 << 3) HIGH in order to configure
	 * the timer as CTC (Clear Timer on Compare Match) where the OCR1A register
	 * will hold the compare value */
	IO_PIN(0x61) = (1 << 2) | (1 << 0) | (1 << 3);

	/* Here we are calculating the compare value to equal the amount of cycles
	 * needed for 1 second in respect to the timer clock speed after the
	 * prescaler, and assigning the value to the OCR1AH and OCR1AL registers */
	uint16_t compare_value = (16000000 / 1024) - 1;
	// OCR1AH
	IO_PIN(0x69) = (compare_value >> 8);
	// OCR1AL
	IO_PIN(0x68) = (compare_value & 0xFF);

	while (1) {
		// infinite loop
	}
}
