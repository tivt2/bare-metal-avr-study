/* 8_i2c */

#include "avr_atmega328p.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BAUD 9600
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

#define OCR0A_MS_64PRE 249 // ((CPU_CLOCK >> 6) / 1000) - 1
void delay_ms(uint16_t ms) {
	if (ms == 0) {
		return;
	}

	// CTC mode
	SET_BIT(TCCR0A, 1);
	// 64 prescaler
	SET_BIT(TCCR0B, 0);
	SET_BIT(TCCR0B, 1);

	// time for a millisecond
	GET_ADDR(OCR0A) = OCR0A_MS_64PRE;
	// clean the timer
	GET_ADDR(TCNT0) = 0;

	do {
		while (!READ_BIT(TIFR0, 1)) {
		}
		SET_BIT(TIFR0, 1);
	} while (--ms);
}

void ERROR() {
	// Built-in LED will be used to indicate Error state
	SET_BIT(DDRB, 5);
	while (1) {
		TOGGLE_BIT(PORTB, 5);
		delay_ms(200);
	}
}

void I2C_start() {
	GET_ADDR(TWCR) = (1 << 2) | (1 << 5) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
	}
	if ((GET_ADDR(TWSR) & 0xF8) != 0x08) {
		USART_println("Error: start");
		ERROR();
	}
}

void I2C_restart() {
	GET_ADDR(TWCR) = (1 << 2) | (1 << 5) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
	}
	if ((GET_ADDR(TWSR) & 0xF8) != 0x10) {
		USART_println("Error: restart");
		ERROR();
	}
}

void I2C_stop() {
	GET_ADDR(TWCR) = (1 << 2) | (1 << 4) | (1 << 7);
	while (READ_BIT(TWCR, 4)) {
	}
}

void I2C_master_transmitter(uint8_t addr) {
	GET_ADDR(TWDR) = (addr << 1);
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
	}
	if ((GET_ADDR(TWSR) & 0xF8) != 0x18) {
		char buf[16] = "addr: ";
		USART_println("Error: master transmitter");
		utoa(addr, &buf[6], 16);
		USART_println(buf);
		ERROR();
	}
}

void I2C_master_receiver(uint8_t addr) {
	GET_ADDR(TWDR) = (addr << 1) | 0x01;
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
	}
	if ((GET_ADDR(TWSR) & 0xF8) != 0x40) {
		char buf[16] = "addr: ";
		USART_println("Error: master receiver");
		utoa(addr, &buf[6], 16);
		USART_println(buf);
		ERROR();
	}
}

void I2C_write(uint8_t data) {
	GET_ADDR(TWDR) = data;
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
	}
	if ((GET_ADDR(TWSR) & 0xF8) != 0x28) {
		char buf[16] = "data: ";
		USART_println("Error: write data");
		utoa(data, &buf[6], 16);
		USART_println(buf);
		ERROR();
	}
}

uint8_t I2C_read() {
	GET_ADDR(TWCR) = (1 << 2) | (1 << 6) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
	}
	if ((GET_ADDR(TWSR) & 0xF8) != 0x50) {
		USART_println("Error: read data");
		ERROR();
	}
	return GET_ADDR(TWDR);
}

uint8_t I2C_read_last() {
	GET_ADDR(TWCR) = (1 << 2) | (1 << 7);
	while (!READ_BIT(TWCR, 7)) {
	}
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

	I2C_start();
	I2C_master_transmitter(mpu6050_addr);
	I2C_write(0x6B);
	I2C_write(0x00);
	I2C_stop();

	while (1) {
		I2C_start();

		I2C_master_transmitter(mpu6050_addr);
		uint8_t mpu6050_gyro_xout_h_addr = 0x43;
		I2C_write(mpu6050_gyro_xout_h_addr);
		I2C_restart();
		I2C_master_receiver(mpu6050_addr);
		uint8_t gyro_x_high = I2C_read();
		uint16_t gyro_x = (gyro_x_high << 8) | I2C_read();
		uint8_t gyro_y_high = I2C_read();
		uint16_t gyro_y = (gyro_y_high << 8) | I2C_read();
		uint8_t gyro_z_high = I2C_read();
		uint16_t gyro_z = (gyro_z_high << 8) | I2C_read_last();
		I2C_stop();

		char msg[64] = "\0";
		char buff[16] = "\0";
		utoa(gyro_x, buff, 10);
		strcpy(&msg[strlen(msg)], "gyro => x: ");
		strcpy(&msg[strlen(msg)], buff);
		utoa(gyro_y, buff, 10);
		strcpy(&msg[strlen(msg)], ", y: ");
		strcpy(&msg[strlen(msg)], buff);
		utoa(gyro_z, buff, 10);
		strcpy(&msg[strlen(msg)], ", z: ");
		strcpy(&msg[strlen(msg)], buff);

		USART_println(msg);
	}

	return 0;
}
