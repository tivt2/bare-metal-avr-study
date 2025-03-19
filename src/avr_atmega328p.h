#ifndef __AVR_ATMEGA328P__
#define __AVR_ATMEGA328P__

#include <stdint.h>

#define CPU_CLOCK 16000000

/* Since we are working with bare metal, when reading an address we need to
 * declare it as volatile or the compiler will not let us control the value of
 * the address directly */
#define GET_ADDR(addr) (*(volatile uint8_t *)addr)

#define SET_BIT(addr, bit)                                                     \
	do {                                                                       \
		GET_ADDR(addr) |= (1 << bit);                                          \
	} while (0)

#define UNSET_BIT(addr, bit)                                                   \
	do {                                                                       \
		GET_ADDR(addr) &= ~(1 << bit);                                         \
	} while (0)

#define TOGGLE_BIT(addr, bit)                                                  \
	do {                                                                       \
		GET_ADDR(addr) ^= (1 << bit);                                          \
	} while (0)

#define READ_BIT(addr, bit) ((GET_ADDR(addr) >> bit) & 0x01)

// INTERRUPTS

/* In order to define a function to be a interrupt handler we must declare it
 * named as __vector_N where N is the vector index in the interrupt vector
 * table, this convention is not well defined in any documentation of the
 * avr-toolchain but we can find that by inspecting the linker scripts and its
 * well known by the AVR community.
 *
 * Also in order to properly define the handler we need to add the signal or
 * interrupt flag and with that the used flag so that the compiler doesnt throw
 * away the function during compilation since it will never be used by the main
 * progam */
#define ISR(vector) __attribute__((signal, used)) void vector(void)

#define INT0_VEC __vector_1
#define INT1_VEC __vector_2
#define TIMER0_OVF_VEC __vector_16
#define ADC_VEC __vector_21

// REGISTERS

#define PINB 0x23
#define DDRB 0x24
#define PORTB 0x25

#define PIND 0x29
#define DDRD 0x2A
#define PORTD 0x2B

#define EIMSK 0x3D
#define EICRA 0x69
#define SREG 0x5F

#define TCCR0A 0x44
#define TCCR0B 0x45
#define OCR0A 0x47
#define TIMSK0 0x6E

#define TCCR1A 0x80
#define TCCR1B 0x81
#define OCR1AL 0x88
#define OCR1AH 0x89

#define ADCL 0x78
#define ADCH 0x79
#define ADCSRA 0x7A
#define ADMUX 0x7C

#endif /* ifndef __AVR_ATMEGA328P__ */
