/* 5_pwm */

#include "avr_atmega328p.h"
#include <stdint.h>

/* To create a smooth fading effect over approximately one second, we need to
 * adjust the duty cycle incrementally and we are going to achieve this by an
 * interrupt routine every time the timer overflows (reaches 0xFF + 1).
 * To achieve a fade duration of 1 second, we trade off a slightly lower maximum
 * power output for more precise timing. By limiting OCR0A (comparisson
 * register) to 250, the duty cycle will have a maximum of ~98.04% instead of
 * 100% power */

/* Calculations:
 * Timer overflows per second: 16 MHz / 256 = 62500 overflows/s
 * Overflows needed per duty cycle step: 62500 / 256 ≈ 244.14 overflows/step
 *
 * After trade-off:
 * Overflows needed per duty cycle step: 62500 / 250 = 250 overflows/step
 */

#define MAX_DUTY_CYCLE 250
#define MAX_OVERFLOW_COUNT (CPU_CLOCK / 256) / MAX_DUTY_CYCLE

volatile uint16_t overflow_count = 0;
volatile uint8_t fade_step = 1;

/* For the timer interrupt we must correctly pass the vector name expected by
 * the compiler to trigger an interrupt on overflow, this is the vector idx=16
 */
ISR(TIMER0_OVF_VEC) {
	// every interrupt we increase the overflow count
	overflow_count++;

	// reaching the value needed to inc/dec the duty cycle
	if (overflow_count >= MAX_OVERFLOW_COUNT) {
		// reset the overflow count
		overflow_count = 0;

		// inc/dec the duty cycle
		GET_ADDR(OCR0A) += fade_step;

		// if the duty cycle reached its maximum or minimum value
		if (GET_ADDR(OCR0A) == 0 || GET_ADDR(OCR0A) == MAX_DUTY_CYCLE) {
			// we invert the fading direction
			fade_step *= -1;
		}
	}
}

int main(void) {
	// Set OC0A (PD6) as OUTPUT, LED pin
	SET_BIT(DDRD, 6);

	/* Setting the COM0A1 pin will configure the timer to operate in
	 * non-inverting mode, this means that when the timer counts up, before
	 * reaching the OCR0A register value it will output HIGH to the OC0A, when
	 * reaching the OCR0A it will then output LOW, it will then output HIGH
	 * again when the timer resets to 0 (overflow) */
	SET_BIT(TCCR0A, 7);

	/* For this example we will be using the full value of the timer meaning it
	 * will operate at fast PWM mode and will only reset its value when overflow
	 * happens (register is full, 0xFF), for this we set the flags WGM00 and
	 * WGM01 */
	SET_BIT(TCCR0A, 0);
	SET_BIT(TCCR0A, 1);

	/* Setting CS00 enables the timer with no prescaler, meaning the timer runs
	 * at the full CPU clock speed (16 Mhz). This will play a role into making
	 * the fade timing more precise to our 1 second constrain */
	SET_BIT(TCCR0B, 0);

	/* Since we are operating the duty cycle logic during an interrupt we must
	 * setup the timer interrupt accordingly, for this we set the SREG I-flag
	 * (interrupt flag) and the timer flag that triggers an interrupt every
	 * overflow */
	SET_BIT(TIMSK0, 0);
	SET_BIT(SREG, 7);

	while (1) {
	}

	return 0;
}
