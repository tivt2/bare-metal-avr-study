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

## Interrupts

Interrupt functionality allows us to pause the current execution of the program and execute a subroutine predefined in the Interrupt Vector Table (IVT), these interrupt subroutines are stored in the table when we setup/flash the firmware of the MCU.

This functionality can be achieved in several ways: by using pins configured to trigger interrupt signals upon meeting specific conditions, such as a rising edge(going from HIGH to LOW), by setting the MCU to trigger interrupts at regular intervals using internal timers that count clock cycles, or by configuring peripherals to trigger an interrupt when they complete a specific task, such as finishing a data transfer or completing an ADC conversion.

These methods enable the MCU to respond to external and internal events in real-time, enhancing its flexibility and performance in handling various tasks.

## Timers

The timer/counter peripheral is a piece of hardware that increments a register based on an input clock cycle, which can be prescaled by the timer itself. This means that the timer can derive its clock from the CPU clock or an external clock source. When using the CPU clock, the timer operates synchronously with the CPU, ensuring reliable timing and event scheduling.

The timer is essentially a counter combined with a comparator. It functions similarly to an Arithmetic Logic Unit (ALU) in that it can compare the current count value with a predefined value. If the current count matches the configured condition, the counter triggers a specific action. For this create this conditions it operates around 3 main values, the BOTTOM value(normaly 0), the current count and the TOP value, that can be either the maximum value or a preset value.

There are three main output actions that can be configured when a timer reaches its condition: toggle a pin, clear a pin, and set a pin. In addition to controlling pin states, the timer can trigger interrupt signals under certain conditions. This capability allows the timer to generate precise time-based events or perform specific tasks at regular intervals, making it a powerful tool.

A timer also have multiple modes of operation, making it possible to operate its output as a PWM signal.

## PWM

A PWM (Pulse Width Modulation) signal is a digital signal that alternates between HIGH and LOW states. The rapid changes between these states allow us to control the duration of cycles that stay HIGH and the amount of time they stay LOW, effectively modulating the output to simulate an analog signal. The proportion of time the signal remains in each state is called the duty cycle.

In the ATmega328P, we can create PWM signals using the timer peripheral, which can be configured in either Fast PWM Mode or Phase Correct PWM Mode. In Fast PWM Mode, the timer counts up until it reaches the TOP value and then resets to the BOTTOM, repeating this process. In Phase Correct PWM Mode, the timer counts up until it reaches the TOP value, then reverses direction and counts down back to the BOTTOM. This mode, as the name indicates, provides more precision, allowing for finer control over the PWM signal.

## ADC(Analog to Digital Converter)

The ADC peripheral in the ATmega328P converts analog signals into digital signals, allowing us to process them within our program. The ADC works by using a reference voltage and measuring the potential difference between this reference voltage and the input voltage. This difference is then converted into a digital value, with a resolution of up to 10 bits in the case of the ATmega328P. This conversion process is handled entirely by the hardware.

To achieve accurate and reliable ADC readings, we must configure the ADC correctly. One key configuration is setting the prescaler value, which determines the division factor for the system clock to produce the ADC clock. The prescaler ensures that the ADC operates within an optimal frequency range, balancing conversion speed and precision.

The ADC can operate in different modes. It can perform continuous conversions, repeatedly converting the analog input without interruption, or it can be configured to start a conversion only when a specific flag is set. Additionally, the ADC can trigger an interrupt when a conversion completes or other specified conditions are met.

## Examples
- 1_blink: This is the classic blink LED example, MCUs normaly have multiplexer systems so the pins of the MCU can be shared depending on the internal peripheral that is interacting with it or the CPU.

- 2_button_polling: Here we setup a button and a LED, where when the button is pressed the LED lights up, to achieve this we use a technique called GPIO polling.

- 3_interrupt: In this example we flicker an LED slowly, we also setup and utilize an external interrupt in order to trigger a routine that flickers an led faster 10 times.

- 4_timers: A timer is configured to toggle a pin connected to an LED, where it will blink it every second.

- 5_pwm: Utilizing the 8-bit timer to generate a PWM signal and fade in/out an LED.

- 6_adc: Reading the analog input from a potentiometer and with it controlling the PWM duty cycle, this PWM signal controls a LED brightness.
