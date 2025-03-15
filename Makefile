.SECONDARY: # all intermediary targets are treated as secondary files

# Dirs
SRC_DIR=src
BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj
BIN_DIR=$(BUILD_DIR)/bin
HEX_DIR=$(BUILD_DIR)/hex

# Toolchain
CC=avr-gcc
OBJCOPY=avr-objcopy
FLASH=avrdude

# Files
SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
BINS=$(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.bin, $(SOURCES))
HEXES=$(patsubst $(SRC_DIR)/%.c, $(HEX_DIR)/%.hex, $(SOURCES))
BASENAMES=$(basename $(notdir $(SOURCES)))

# Flags
CLOCK=16000000
MCU=atmega328p

WARNING_FLAGS=-Wall -Wextra -Werror -Wshadow
CFLAGS=-Os -mmcu=$(MCU) -DF_CPU=$(CLOCK) $(WARNING_FLAGS)
LFLAGS=-mmcu=$(MCU) $(WARNING_FLAGS)
HEXFLAGS=-O ihex -R .eeprom

FLASH_PORT=/dev/ttyUSB0
FLASH_FLAGS=-F -V -c arduino -p ATMEGA328P -P $(FLASH_PORT) -b 115200

# Phonies
# mark phonies as commands even if there is files with same name
.PHONY: all clean

all: $(HEXES)

clean:
	$(RM) -r $(BUILD_DIR)

# Build

# (target): [prerequisite...]
# 	command execution

# $@ = target
# $< = first prerequisite
# $^ = prerequisite...
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables

## Hex-format
$(HEX_DIR)/%.hex: $(BIN_DIR)/%.bin
	@mkdir -p $(dir $@)
	$(OBJCOPY) $(HEXFLAGS) $< $@

## Linker
$(BIN_DIR)/%.bin: $(OBJ_DIR)/%.o
	@mkdir -p $(dir $@)
	$(CC) $(LFLAGS) $< -o $@

## Compiler
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Flashing
$(BASENAMES): $(HEXES)
	sudo $(FLASH) $(FLASH_FLAGS) -U flash:w:$(HEX_DIR)/$@.hex
