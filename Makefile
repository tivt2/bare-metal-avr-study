.SECONDARY: # all targets are treated as secondary files

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
TARGETS=$(patsubst $(SRC_DIR)/%.c, %, $(SOURCES))
OBJECTS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
BINS=$(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.bin, $(SOURCES))
HEXES=$(patsubst $(SRC_DIR)/%.c, $(HEX_DIR)/%.hex, $(SOURCES))

# Flags
CLOCK=16000000
MCU=atmega328p

WARNING_FLAGS=-Wall -Wextra -Werror -Wshadow
CFLAGS=-Os -mmcu=$(MCU) -DF_CPU=$(CLOCK) $(WARNING_FLAGS)
LFLAGS=-mmcu=$(MCU) $(WARNING_FLAGS)
HEXFLAGS=-O ihex -R .eeprom

FLASH_PORT=/dev/ttyUSB0
FLASH_FLAGS=-F -V -c arduino -p ATMEGA328P -P $(FLASH_PORT) -b 115200

# Build
## Hex-format
$(HEXES): $(BINS)
	@mkdir -p $(dir $@)
	$(OBJCOPY) $(HEXFLAGS) $^ $@

## Linker
$(BIN_DIR)/%.bin: $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(LFLAGS) -o $@ $^

## Compiler
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

# Phonies
.PHONY: all clean flash

all: $(HEXES)

clean:
	$(RM) -r $(BUILD_DIR)

$(TARGETS): $(HEXES)
	sudo $(FLASH) $(FLASH_FLAGS) -U flash:w:$(HEX_DIR)/$@.hex
