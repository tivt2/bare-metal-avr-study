# AVR(ATmega328P) bare-metal

This is a learning exploration on how to code bare-metal using the C language and also learn about MCUs using the AVR(ATmega328P), most of the information used in the learning process comes from the [ATmega328P datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf). Keep in mind that this is just a "learning diary".

This project will be separated into enumerated concepts with its corresponding example, where each example is built individualy, the build system is a simple makefile utilizing the avr-toolchain to compile and flash to the mcu.

## GPIO

In MCUs like the ATmega328P, GPIO (General-Purpose Input/Output) pins are components that interfaces with external devices, each GPIO pin can be configured as either an input or an output, and this data direction controled by the Data Direction Register (DDR) that lives only in software.

The ATmega328P MCU has a dedicated set of GPIO pins organized into ports in software, such as PORTB, each pin in the port is associated with a bit in the DDR register and mapped to a physical PIN in the MCU.

To configure a PIN as input we write HIGH(1) to the associated DDR bit and to configure it as an output we write LOW(0) to that bit, this software configuration reflects in the physical interface of the pin.

When a pin is configured as an output, writing a logical HIGH(1) to the corresponding bit in the PORT register sets the physical pin to a high voltage level, while writing a logical LOW(0) sets it to a low voltage level. This allows the MCU to control external components such as LEDs, motors, or relays.

Additionally, the MCU introduces a concept known as pin multiplexing. In many MCUs, including the ATmega328P, GPIO pins can be shared by multiple internal peripherals or functions through the use of a multiplexer. This means that a single physical pin can be connected to different internal registers or peripherals, depending on the configuration set by the MCU firmware.

For example, a GPIO pin may be used as a digital input in one configuration, but it can also be configured to serve as the clock input for a timer/counter peripheral in another configuration. The multiplexer allows the MCU to dynamically allocate pin functionality, providing flexibility in utilizing the limited number of GPIO pins available on the chip.

## GPIO polling

The idea of GPIO polling is such as we monitor the state of a input pin. This technique the MCU constantly checks the state of the pin that was configured as an input pin, this makes the MCU have direct access to a hardware device where it can check its state during code execution.

One limitation of GPIO polling is that it may introduce latency in the system where real-time responses to hardware events are necessary, in those cases we can use interrupt signals to achieve a more time-sensitive approach.

A disavantage of GPIO polling compared to an interrupt system is the consumption of CPU cycles every time the pin value needs to be checked. In some sense GPIO polling is "slow" if compared to a system that levarages interrupt signal to change the program execution. In counter part GPIO polling makes the program deterministic and predictable.

## Examples
- 1_blink: This is the classic blink LED example, MCUs normaly have multiplexer systems so the pins of the MCU can be shared depending on the internal peripheral that is interacting with it or the CPU.

- 2_button_polling: Here we setup a button and a LED, where when the button is pressed the LED lights up, to achieve this we use a technique called GPIO polling.
