/* 6_adc */

#include "avr_atmega328p.h"
#include <stdint.h>

ISR(ADC_VEC) {
	/* As stated in the datasheet in order to correctly read the 10-bits of
	 * ADC value, we must first read ADCL then ADCH */
	uint8_t adc_low = GET_ADDR(ADCL);
	uint16_t adc_read = ((uint16_t)GET_ADDR(ADCH) << 8) | adc_low;

	/* Since in this example we are using the 8-bit timer to generate a PWM
	 * signal, we must map the adc_read that is in the range 0..1023
	 * to the range 0..255 to correctly input a valid duty cycle in the
	 * OCR0A register
	 *
	 * Calculations for mapping:
	 * 1. Arithmethic: (adc_read * 256)/1024
	 * 2. Bit shifting: adc_read >> 2
	 *
	 * The shifting method works because shifting right by 2 bits is equivalent
	 * to dividing by 4, and 256/1024 simplifies to 1/4
	 * */
	uint8_t duty_cycle = adc_read >> 2;

	GET_ADDR(OCR0A) = duty_cycle;
}

int main(void) {
	// OC0A (PD6) configured as OUTPUT, LED pin
	SET_BIT(DDRD, 6);

	/* Setting 8-bit timer with COM0A1 flag (clear OC0A on compare match),
	 * WGM00 and WGM01 flags so the wave form is configured as fast PWM, and
	 * will overflow when reaching 0xFF (TOP) */
	SET_BIT(TCCR0A, 0);
	SET_BIT(TCCR0A, 1);
	SET_BIT(TCCR0A, 7);

	// Flag CS00 so the PWM uses the CPU clock without any prescaling
	SET_BIT(TCCR0B, 0);

	/* Setting the REFS0 flag of ADMUX, will configure the ADC reference voltage
	 * as the MCU voltage(5V), no MUX flag is being set this means that we will
	 * read the ADC0 (PC0) input pin */
	SET_BIT(ADMUX, 6);

	/* Since we will be using the CPU clock as the conversion clock its
	 * necessary to scale it for a precise conversion, we are setting flags
	 * ADPS0, ADPS1, ADPS2 to configure the prescaler as 128, giving us the
	 * conversion clock of 16Mhz / 128 = 125Khz */
	SET_BIT(ADCSRA, 0);
	SET_BIT(ADCSRA, 1);
	SET_BIT(ADCSRA, 2);

	/* Configuring the ADC as auto trigger by setting the flag ADATE, because we
	 * will not change any ADTS flags of the ADCSRB register the auto trigger is
	 * configured as free running (continuosly convert the input) */
	SET_BIT(ADCSRA, 5);

	/* Configuring the ADC to execute an interrupt routine when a conversion is
	 * done, by setting the flags ADIE (ADC interrupt enable) and SREG register
	 * (global interrupt enable) */
	SET_BIT(ADCSRA, 3);
	SET_BIT(SREG, 7);

	// The flag ADEN of ADCSRA register in used to start the ADC
	SET_BIT(ADCSRA, 7);
	// Setting the flag ADSC will start the first convertion
	SET_BIT(ADCSRA, 6);

	while (1) {
	}

	return 0;
}
