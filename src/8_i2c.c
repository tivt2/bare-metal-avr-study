/* 8_i2c */

#include "avr_atmega328p.h"
#include <stdlib.h>
#include <string.h>

// #define BAUD 9600
#define UBRR 103 // ((CPU_CLOCK / 16 / BAUD) - 1)

uint8_t usart_initialized = 0;
void USART_init() {
	// Setting UBRR value so the BAUD rate is correct
	GET_ADDR(UBRR0L) = UBRR;
	GET_ADDR(UBRR0H) = UBRR >> 8;

	// Configure the transmissing data size as 8-bit
	SET_BIT(UCSR0C, 1);
	SET_BIT(UCSR0C, 2);

	// Enable the USART transmitter
	SET_BIT(UCSR0B, 3);
	usart_initialized = 1;
}

void USART_write_byte(uint8_t byte) {
	if (!usart_initialized) {
		return;
	}
	while (!READ_BIT(UCSR0A, 5)) {
		// wait UDREn be HIGH to indicate transmitter register to be empty
	}

	// write data
	GET_ADDR(UDR0) = byte;
}

void USART_println(char *str) {
	if (!usart_initialized) {
		return;
	}
	// iterate over the string and transmit byte by byte
	while (*str) {
		USART_write_byte(*str++);
	}
	USART_write_byte('\r');
	USART_write_byte('\n');
}

#define OCR0A_MS_64PRESCALER 249 // ((CPU_CLOCK >> 6) / 1000) - 1
void delay_ms(uint16_t ms) {
	if (ms == 0) {
		return;
	}

	// 8-bit Timer/Counter
	// CTC mode
	SET_BIT(TCCR0A, 1);
	// 64 prescaler
	SET_BIT(TCCR0B, 0);
	SET_BIT(TCCR0B, 1);

	// time for a millisecond
	GET_ADDR(OCR0A) = OCR0A_MS_64PRESCALER;
	// clean the timer
	GET_ADDR(TCNT0) = 0;

	do {
		while (!READ_BIT(TIFR0, 1)) {
			// Wait until the TIFR0 flag be marked HIGH
			// TIFR0 is set HIGH when timer reach comparisson condition
		}
		// Setting TIFR0 will toggle it
		SET_BIT(TIFR0, 1);
	} while (--ms);
}

void ERROR() {
	// Built-in LED will be used to indicate Error state
	// by blinking fast
	SET_BIT(DDRB, 5);
	while (1) {
		TOGGLE_BIT(PORTB, 5);
		delay_ms(200);
	}
}

// I2C "driver" starts here

void I2C_start() {
	/* Configure master start condition flags TWEN, TWSTA and TWINT */
	GET_ADDR(TWCR) = (1 << 2) | (1 << 5) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
		// Wait for TWINT be set as HIGH
	}
	/* Read TWSR for status code, if not START condition has been transmitted
	 * error out */
	if ((GET_ADDR(TWSR) & 0xF8) != 0x08) {
		USART_println("Error: start");
		ERROR();
	}
}

void I2C_restart() {
	/* Configure master repeat start condition flags TWEN, TWSTA and TWINT */
	GET_ADDR(TWCR) = (1 << 2) | (1 << 5) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
		// Wait for TWINT be set as HIGH
	}
	/* Read TWSR for status code, if not repeated START condition has been
	 * transmitted error out */
	if ((GET_ADDR(TWSR) & 0xF8) != 0x10) {
		USART_println("Error: restart");
		ERROR();
	}
}

void I2C_stop() {
	/* Configure master stop condition flags TWEN, TWSTO and TWINT */
	GET_ADDR(TWCR) = (1 << 2) | (1 << 4) | (1 << 7);
	while (READ_BIT(TWCR, 4)) {
		// Wait for TWSTO be set as LOW
		// indicating comms stoped
	}
}

void I2C_master_transmitter(uint8_t addr) {
	/* Load TWDR with device address and least significant bit set as 0 */
	GET_ADDR(TWDR) = (addr << 1);
	// Configure master transmitter condition flags TWEN and TWINT
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
		// Wait for TWINT be set as HIGH
	}
	/* Read TWSR for status code, if not SLA+W condition has been
	 * transmitted and ACK has been received error out */
	if ((GET_ADDR(TWSR) & 0xF8) != 0x18) {
		char buf[16] = "addr: ";
		USART_println("Error: master transmitter");
		utoa(addr, &buf[6], 16);
		USART_println(buf);
		ERROR();
	}
}

void I2C_master_receiver(uint8_t addr) {
	/* Load TWDR with device address and least significant bit set as 1 */
	GET_ADDR(TWDR) = (addr << 1) | 0x01;
	// Configure master receiver condition flags TWEN and TWINT
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
		// Wait for TWINT be set as HIGH
	}
	/* Read TWSR for status code, if not SLA+R condition has been
	 * transmitted and ACK has been received error out */
	if ((GET_ADDR(TWSR) & 0xF8) != 0x40) {
		char buf[16] = "addr: ";
		USART_println("Error: master receiver");
		utoa(addr, &buf[6], 16);
		USART_println(buf);
		ERROR();
	}
}

void I2C_write(uint8_t data) {
	// Load TWDR with data to be trasmitted
	GET_ADDR(TWDR) = data;
	// Configure transmittion condition flags TWEN and TWINT
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
		// Wait for TWINT be set as HIGH
	}
	/* Read TWSR for status code, if not data byte has been
	 * transmitted and ACK has been received error out */
	if ((GET_ADDR(TWSR) & 0xF8) != 0x28) {
		char buf[16] = "data: ";
		USART_println("Error: write data");
		utoa(data, &buf[6], 16);
		USART_println(buf);
		ERROR();
	}
}

uint8_t I2C_read() {
	// Configure receiving condition flags TWEN, TWEA and TWINT
	GET_ADDR(TWCR) = (1 << 2) | (1 << 6) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
		// Wait for TWINT be set as HIGH
	}
	/* Read TWSR for status code, if not data byte has been
	 * received and ACK has been returned error out */
	if ((GET_ADDR(TWSR) & 0xF8) != 0x50) {
		USART_println("Error: read data");
		ERROR();
	}
	return GET_ADDR(TWDR);
}

uint8_t I2C_read_last() {
	// Configure receiving last byte condition flags TWEN and TWINT
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
		// Wait for TWINT be set as HIGH
	}
	/* Read TWSR for status code, if not data byte has been
	 * received and NOT ACK has been returned error out */
	if ((GET_ADDR(TWSR) & 0xF8) != 0x58) {
		USART_println("Error: read last data");
		ERROR();
	}
	return GET_ADDR(TWDR);
}

int main(void) {
	USART_init();
	USART_println("Hello from ATmega328P");

	// MPU6050 I2C address
	uint8_t mpu6050_addr = 0x68;

	uint32_t desired_scl = 400000; // MPU6050 fast-mode - 400kHz
	uint8_t prescaler = 1;

	/* As we see in the ATmega328P data sheet, the Bit Rate Generator Unit
	 * define the SCL frequency as:
	 * SCL = (CPU_CLOCK) / (16 + 2 * TWBR * prescaler)
	 * So we get the formula to calculate TWBR as:
	 * TWBR = ((CPU_CLOCK / SCL) - 16) / 2 * prescaler */
	uint8_t twbr = ((uint8_t)(CPU_CLOCK / desired_scl) - 16) / 2 * prescaler;
	GET_ADDR(TWBR) = twbr;

	/* MPU6050 addresses for PWOR_1 configuration register and GYRO_XOUT_H
	 * register */
	uint8_t MPU6050_PWOR_1_addr = 0x6B;
	uint8_t MPU6050_GYRO_XOUT_H_addr = 0x43;

	/* I2C communication to configure the MPU6050 module, this module starts
	 * operating in low power mode and must reset this condition in order to
	 * correctly read the gyro or accelerometer data */

	/* I2C Communication order of operations:
	 * - Transmit START condition
	 * - Transmit SLA+W and wait ACK
	 * - Transmit RA and wait ACK
	 * - Transmit DATA and wait ACK
	 * - Transmit STOP condition
	 *
	 * The RA = register address and DATA is the configuration byte we want to
	 * set for the MPU6050 PWER_1 register */
	I2C_start();
	I2C_master_transmitter(mpu6050_addr);
	I2C_write(MPU6050_PWOR_1_addr);
	I2C_write(0x00);
	I2C_stop();

	/* In the main loop we will pool the gyro information, most modules that
	 * communication with I2C will increment the transmitted RA everytime a read
	 * operation occurs */
	while (1) {
		/* I2C Communication order of operations:
		 * - Transmit START condition
		 * - Transmit SLA+W and wait ACK
		 * - Transmit RA and wait ACK
		 * - Transmit repeated START condition
		 * - Transmit SLA+R and wait ACK
		 * - Receive DATA from RA and return ACK
		 * - Receive DATA from RA+1 and return ACK
		 * - Receive DATA from RA+2 and return ACK
		 * - Receive DATA from RA+3 and return ACK
		 * - Receive DATA from RA+4 and return ACK
		 * - Receive DATA from RA+5 and return NOT ACK
		 * - Transmit STOP condition
		 * */
		I2C_start();
		I2C_master_transmitter(mpu6050_addr);
		I2C_write(MPU6050_GYRO_XOUT_H_addr);
		I2C_restart();
		I2C_master_receiver(mpu6050_addr);
		// MPU6050 gyro values are 16-bit 2's complement
		uint8_t gyro_x_high = I2C_read();
		int16_t gyro_x = (gyro_x_high << 8) | I2C_read();
		uint8_t gyro_y_high = I2C_read();
		int16_t gyro_y = (gyro_y_high << 8) | I2C_read();
		uint8_t gyro_z_high = I2C_read();
		int16_t gyro_z = (gyro_z_high << 8) | I2C_read_last();
		I2C_stop();

		/* MPU6050 gyro is configured by default as -+250dgre/s, this will
		 * convert the 2's complement value into dgree/s value */
		float gyro_x_dgre = gyro_x / 32768.0 * 250.0;
		float gyro_y_dgre = gyro_y / 32768.0 * 250.0;
		float gyro_z_dgre = gyro_z / 32768.0 * 250.0;

		/* String formatting to send a single line with the gyro information via
		 * USART */
		char msg[64] = "\0";
		char buff[16] = "\0";
		dtostrf(gyro_x_dgre, 5, 2, buff);
		strcpy(&msg[strlen(msg)], "x: ");
		strcpy(&msg[strlen(msg)], buff);
		dtostrf(gyro_y_dgre, 5, 2, buff);
		strcpy(&msg[strlen(msg)], ", y: ");
		strcpy(&msg[strlen(msg)], buff);
		dtostrf(gyro_z_dgre, 5, 2, buff);
		strcpy(&msg[strlen(msg)], ", z: ");
		strcpy(&msg[strlen(msg)], buff);

		USART_println(msg);
	}

	return 0;
}
