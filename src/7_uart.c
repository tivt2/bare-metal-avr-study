/* 7_uart
 * In this example, we utilize the builtin usart of the ATmega328P to send the
 * value of a potentiometer via usb */

#include "stdlib.h"
#include <inttypes.h>

#define IO_REG_OFFSET 0x20
#define IO_PIN(addr) (*(volatile uint8_t *)(addr + IO_REG_OFFSET))

// ADC registers
#define ADMUX IO_PIN(0x5C)
#define ADCSRA IO_PIN(0x5A)
#define ADCH IO_PIN(0x59)
#define ADCL IO_PIN(0x58)

// USART registers
#define UBRR0H IO_PIN(0xA5)
#define UBRR0L IO_PIN(0xA4)
#define UCSR0A IO_PIN(0xA0)
#define UCSR0B IO_PIN(0xA1)
#define UCSR0C IO_PIN(0xA2)
#define UDR0 IO_PIN(0xA6)

void ADC_init(void) {
	// ADC initializer as explained in the ADC example
	ADMUX = (1 << 6);
	ADCSRA = (1 << 7) | (1 << 2) | (1 << 1) | (1 << 0);
}

uint16_t ADC_read(uint8_t adc_pin_num) {
	// set the ADMUX to read the correct selected adc pin number
	ADMUX = (ADMUX & 0xF0) | (adc_pin_num & 0x0F);

	// ADC read explained in the ADC example
	ADCSRA |= (1 << 6);
	while ((ADCSRA & (1 << 6)) != 0) {
		// wait until ADSC flag is set to LOW
	}

	return (uint16_t)ADCL | ((uint16_t)ADCH << 8);
}

void USART_transmit(uint8_t byte) {
	/* In order to transmit new data we first must check if the register that
	 * holds the data is able to receive new data, if the UDREn(1 << 5) flag is
	 * HIGH the buffer is ready to receive data */
	while (!(UCSR0A & (1 << 5))) {
		// wait transmitter register to be empty
	}

	/* This is the register that holds the data(buffer) */
	UDR0 = byte;
}

void USART_transmit_string(char *str) {
	// iterate over the string and transmit byte by byte
	while (*str) {
		USART_transmit(*str++);
	}
}

#define F_CPU 16000000
#define BAUD 9600

int main(void) {
	ADC_init();

	/* USART have multiple modes of operation where each mode needs a different
	 * formula to calculate the UBRR value that will be used, in this case we
	 * are using the Asnchronous Normal Mode */
	uint16_t ubrr = (F_CPU / 16 / BAUD) - 1;

	/* In order to syncronize the two elements communicating via USART, we
	 * define a UBRR value that share a baud rate and will be requested to be
	 * the same at both the elements */
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	/* Since we are only transmitting data from the ATmega328P MCU via usb, only
	 * the TXEN0(1 << 3) need to be set HIGH, if needed we can also set HIGH the
	 * RXEN0(1 << 4) to receive data */
	UCSR0B = (1 << 3);

	/* The USART peripheral have the ability to receive diferent sizes of data,
	 * here we set it to transmitt 8-bit data, but we could also set 5, 6, 7, 9
	 * bits of data */
	UCSR0C = (1 << 2) | (1 << 1);

	USART_transmit_string("Hello from ATmega328P\r\n");

	// buffer used to store the potentiometer value as text
	char buf[8 * sizeof(uint16_t) + 1];
	while (1) {
		// read analog signal from potentiometer
		uint16_t pot_val = ADC_read(0);
		// convert potentiometer value into string
		utoa(pot_val, buf, 10);

		// transmit data
		USART_transmit_string(buf);
		USART_transmit_string("\r\n");

		/* To receive the data correctly, one must use a tool such as `screen
		 * /dev/ttyUSB0 9600` where 'screen' is the tool '/dev/ttyUSB0' is the
		 * PORT the ATmega328P is connected to and '9600' is the shared BAUD
		 * rate used by both parties, Ctrl+A -> k -> y, to quit */
	}
}
