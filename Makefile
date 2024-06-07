CC=avr-gcc
CFLAGS=-Wall -Wextra -Werror

OBJCOPY=avr-objcopy
FLASH=avrdude
FLASH_PORT=/dev/ttyUSB0

CLOCK=16000000
MMCU=atmega328p

SRC_DIR=src
BUILD_DIR=build

ifndef arg
	$(error Please provide the 'arg=example_name')
endif

file:
	$(if $(wildcard ($(SRC_DIR)/$(arg).c)),\
		$(error File $(SRC_DIR)/$(arg).c does not exist))
	@mkdir -p $(BUILD_DIR)/$(arg)
	$(CC) $(CFLAGS) -Os -DF_CPU=$(CLOCK) -mmcu=$(MMCU) -c -o $(BUILD_DIR)/$(arg)/$(arg).o $(SRC_DIR)/$(arg).c
	$(CC) $(CFLAGS) -mmcu=$(MMCU) -o $(BUILD_DIR)/$(arg)/$(arg).bin $(BUILD_DIR)/$(arg)/$(arg).o
	$(OBJCOPY) -O ihex -R .eeprom $(BUILD_DIR)/$(arg)/$(arg).bin $(BUILD_DIR)/$(arg)/$(arg).hex

flash:
	$(if $(wildcard ($(BUILD_DIR)/$(arg)/$(arg).hex)),\
		$(error Hex file $(BUILD_DIR)/$(arg)/$(arg).hex does not exist))
	sudo $(FLASH) -F -V -c arduino -p ATMEGA328P -P $(FLASH_PORT) -b 115200 -U flash:w:$(BUILD_DIR)/$(arg)/$(arg).hex

clear:
	$(if $(wildcard ($(BUILD_DIR)/$(arg)/$(arg).hex)),\
		$(error Folder $(BUILD_DIR)/$(arg) does not exist))
	@rm -rf $(BUILD_DIR)/$(arg)
