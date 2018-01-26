MCU=atmega32u4

F_CPU=16000000

TARGET=atreus

SWAPCOLUMNS=no
# set to allow for flipping the PCB

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  USB ?= `ls /dev/tty.usb*`
else
  USB ?= COM7
endif

# Build your keyboard layout
#
#   make build
build: $(TARGET).hex

# Upload your keyboard layout to your keyboard.  You will need to reset your
# keyboard for the upload to complete
#
#   make upload
upload: $(TARGET).hex
	# while [ ! -r $(USB) ]; do sleep 5; done;
	avrdude -p $(MCU) -c avr109 -U flash:w:$(TARGET).hex -P $(USB)

# remove build files
clean:
	-rm -f $(TARGET) $(TARGET).hex *.o

$(TARGET).o: layout.h
usb_keyboard.o: usb_keyboard.h

$(TARGET): $(TARGET).o usb_keyboard.o
	avr-gcc -mmcu=$(MCU) usb_keyboard.o $(TARGET).o -o $(TARGET)

$(TARGET).hex: $(TARGET)
	avr-size $(TARGET)
	avr-objcopy -O ihex -R .eeprom $(TARGET) $(TARGET).hex

%.o: %.c
ifeq ($(SWAPCOLUMNS),yes)
	avr-gcc -std=gnu99 -DSWAPCOLUMNS=$(SWAPCOLUMNS) -Os -D F_CPU=$(F_CPU)UL -mmcu=$(MCU) -c -o $@ $<
else
	avr-gcc -std=gnu99 -Os -D F_CPU=$(F_CPU)UL -mmcu=$(MCU) -c -o $@ $<
endif
