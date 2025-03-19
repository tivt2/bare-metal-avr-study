/* 7_usart */

#include "avr_atmega328p.h"
#include <stdint.h>
#include <stdlib.h>

void ADC_init(void) {
	// Set AVcc as the reference voltage by setting the flag REFS0
	SET_BIT(ADMUX, 6);

	/* Set flags ADPS0, ADPS1 and ADPS2 to configure the prescaler
	 * and correctly configure the conversion clock to 16Mhz/128 = 125Khz */
	SET_BIT(ADCSRA, 0);
	SET_BIT(ADCSRA, 1);
	SET_BIT(ADCSRA, 2);

	// Enable ADC auto trigger flag
	SET_BIT(ADCSRA, 5);
	// Enable the ADC
	SET_BIT(ADCSRA, 7);

	// Start first conversion by setting ADSC
	SET_BIT(ADCSRA, 6);
}

uint16_t ADC_read(uint8_t adc_pin_num) {
	/* Set ADMUX mux flags correctly to read the selected ADC_pin */
	uint8_t admux = GET_ADDR(ADMUX);
	GET_ADDR(ADMUX) = (admux & 0xF0) | (adc_pin_num & 0x0F);

	while (READ_BIT(ADCSRA, 4) != 1) {
		// wait until ADIF flag is set to HIGH
		// this marks that a conversion was made
	}

	uint16_t digital_val = GET_ADDR(ADCL);
	digital_val |= GET_ADDR(ADCH) << 8;

	// Setting the ADIF flag will toggle it back to LOW
	SET_BIT(ADCSRA, 4);

	return digital_val;
}

void USART_transmit(uint8_t byte) {
	/* In order to be able to write data to the USART trasmit data buffer, we
	 * must check the UDREn flag and wait it to be HIGH */
	while (!READ_BIT(UCSR0A, 5)) {
		// wait transmitter register to be empty
	}

	/* This is the register that holds the data(buffer) */
	GET_ADDR(UDR0) = byte;
}

void USART_transmit_string(char *str) {
	// iterate over the string and transmit byte by byte
	while (*str) {
		USART_transmit(*str++);
	}
}

#define BAUD 9600

int main(void) {
	/* Calculating the UBRRn value to be used in order to configure the BAUD
	 * rate we will use the Asynchronous normal mode, (U2Xn = 0) */
	uint16_t ubrr = (CPU_CLOCK / 16 / BAUD) - 1;

	// Set the registers that hold the UBRR value
	GET_ADDR(UBRR0L) = ubrr;
	GET_ADDR(UBRR0H) = (ubrr >> 8);

	/* As stated in the data sheet, it asks to always set the DORn bit when
	 * writing to UCSRnA this flag is responsible to inform that an overrun
	 * condition was detected */
	SET_BIT(UCSR0A, 3);

	/* Because we are only sending data from the MCU, we only need to set the
	 * TXENn flag, this will turn only enable the transmitter */
	SET_BIT(UCSR0B, 3);

	/* The USART peripheral have the ability to receive diferent sizes of data,
	 * here we set it to transmitt 8-bit data, but we could also set 5, 6, 7, 9
	 * bits of data */
	/* The USART can be configured to receive/transmit 5, 6, 7, 8 or 9 bits, by
	 * configuring the UCSZn0, UCSZn1 and UCSZn2 flags, we are setting it to
	 * 8-bit for simplicity, for this we set UCSZn0 and UCSZn1 */
	SET_BIT(UCSR0C, 1);
	SET_BIT(UCSR0C, 2);

	USART_transmit_string("Hello from ATmega328P\r\n");

	// ADC initialization
	ADC_init();

	// buffer used to store the potentiometer value as a string
	char buff[8];

	uint16_t last_pot_val = 0;
	while (1) {
		// Read analog signal from potentiometer
		uint16_t pot_val = ADC_read(0);

		// Only send data if potentiometer value changed
		if (pot_val != last_pot_val) {
			last_pot_val = pot_val;

			// Convert potentiometer value into string
			utoa(pot_val, buff, 10);

			// Send data
			USART_transmit_string(buff);
			USART_transmit_string("\r\n");
		}
	}

	return 0;
}
