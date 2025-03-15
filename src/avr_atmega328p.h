#ifndef __AVR_ATMEGA328P__
#define __AVR_ATMEGA328P__

#include <stdint.h>

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

// REGISTERS

#define PINB 0x23
#define DDRB 0x24
#define PORTB 0x25

#define PIND 0x29
#define DDRD 0x2A
#define PORTD 0x2B

#endif /* ifndef __AVR_ATMEGA328P__ */
