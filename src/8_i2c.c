/* 8_i2c */

#include "avr_atmega328p.h"

#define BAUD 9600
#define UBRR ((CPU_CLOCK / 16 / BAUD) - 1)

void USART_init() {
	// Setting UBRR value so the BAUD rate is correct
	GET_ADDR(UBRR0L) = UBRR;
	GET_ADDR(UBRR0H) = UBRR >> 8;

	// Configure the transmissing data size as 8-bit
	SET_BIT(UCSR0C, 1);
	SET_BIT(UCSR0C, 2);

	// Enable the USART transmitter
	SET_BIT(UCSR0B, 3);
}

void USART_write_byte(uint8_t byte) {
	while (!READ_BIT(UCSR0A, 5)) {
		// wait UDREn be HIGH to indicate transmitter register to be empty
	}

	// write data
	GET_ADDR(UDR0) = byte;
}

void USART_print(char *str) {
	// iterate over the string and transmit byte by byte
	while (*str) {
		USART_write_byte(*str++);
	}
}

int main(void) {
	USART_init();

	USART_print("Hello from ATmega328P\r\n");
	while (1) {
		// infinite loop
	}

	return 0;
}
