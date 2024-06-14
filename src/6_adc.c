/* 6_adc
 * In this example, we read the analog input from a potentiometer, and control
 * the brightness of an LED with it */

#include <inttypes.h>

#define IO_REG_OFFSET 0x20
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

#define DDRD IO_PIN(0x0A)

// 8-bit timer registers
#define TCCR0A IO_PIN(0x24)
#define TCCR0B IO_PIN(0x25)
#define OCR0A IO_PIN(0x27)

// ADC registers
#define ADMUX IO_PIN(0x5C)
#define ADCSRA IO_PIN(0x5A)
#define ADCH IO_PIN(0x59)
#define ADCL IO_PIN(0x58)

int main(void) {
	// LED data direction pin as OUTPUT
	DDRD |= (1 << 6);

	/* In the ADMUX register we set the flag REFS0(1 << 6) in order to configure
	 * the ADC reference voltage to AVcc, which will use the voltage of the own
	 * MCU(5V) */
	ADMUX = (1 << 6);
	/* The flag ADEN(1 << 7) present in the ADCSRA is resposible for enabling
	 * the Analog to Digital Converter, we also have the prescaler present
	 * configuration present in this register, here we set flags ADPS2(1 << 2),
	 * ADPS1(1 << 1) and ADPS0(1 << 0) in order to set the prescaler to a
	 * division factor of 128 */
	ADCSRA = (1 << 7) | (1 << 2) | (1 << 1) | (1 << 0);

	// PWM timer configuration
	TCCR0A = (1 << 7) | (1 << 1) | (1 << 0);
	TCCR0B = (1 << 0) | (1 << 2);

	while (1) {
		/* The ADSC(1 << 6) flag initializes the ADC conversion, where it will
		 * utilize some clock cycles to correctly update the ADC register, which
		 * will hold the digital value referent to the analog input, after the
		 * read is done this flag will turn to 0 */
		ADCSRA |= (1 << 6);
		while ((ADCSRA & (1 << 6)) != 0) {
			// wait until ADSC flag is set to LOW
		}
		/* As stated in the datasheet in order to correctly read the 10-bits of
		 * ADC value, we must first read ADCL then ADCH */
		uint16_t adc_read = (uint16_t)ADCL | ((uint16_t)ADCH << 8);

		/* since in this example we are using the 8-bit timer to generate a pwm
		 * signal, we must map the adc_read that is in the range 0..1023
		 * into the range 0..255 to correctly input the duty cycle in the OCR0A
		 * register */
		uint16_t duty_cycle = ((uint32_t)adc_read * 255) / 1023;

		OCR0A = duty_cycle;
	}
}
