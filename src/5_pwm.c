/* 5_pwm
 * In this example, we fade a LED in and out utilizing a PWM signal coming from
 * the 8-bit timer/counter0 and a simple for loop as a delay mechanism */

#include <inttypes.h>

#define IO_REG_OFFSET 0x20
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

#define DDRD IO_PIN(0x0A)

#define TCCR0A IO_PIN(0x24)
#define TCCR0B IO_PIN(0x25)
#define OCR0A IO_PIN(0x27)

int main(void) {
	// LED data direction pin as OUTPUT
	DDRD |= (1 << 6);

	/* the timer configuration is set to Fast PWM Mode, and we also set the OC0A
	 * on compare match and clear the pin at BOTTOM(inverting mode) */
	TCCR0A = (1 << 7) | (1 << 6) | (1 << 1) | (1 << 0);
	TCCR0B = (1 << 0) | (1 << 2);

	/* the OCR0A in this setup controls the duty cycle of the pwm, where 255 is
	 * 100% and 0 is 0% */
	OCR0A = 0;

	/* the main loop is setup in a way that every loop cycle we
	 * increment/decrement the duty cycle, and check if the maximum/minum was
	 * reached in order to revert the direction, also we "delay" some arbitrary
	 * amount of time using a simple for loop in order to be a visible change in
	 * light intensity */
	uint8_t step = 1;
	while (1) {
		OCR0A += step;

		if (OCR0A == 255 || OCR0A == 0) {
			step = -step;
		}

		for (int i = 0; i < 10000; i++) {
		}
	}
}
