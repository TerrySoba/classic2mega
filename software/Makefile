CC      = avr-gcc
CFLAGS  = -Wall -pedantic -Os -std=c99 -Werror
DEVICE  = atmega8
F_CPU   = 8000000	# in Hz
COMPILE = $(CC) $(CFLAGS) -DF_CPU=$(F_CPU) -mmcu=$(DEVICE)
# AVRDUDE = avrdude -c stk500v2 -p m8 -P /dev/ttyACM0
AVRDUDE = avrdude -c usbasp-clone -p m8 -P /dev/ttyACM0
# AVRDUDE = avrdude -c usbasp-clone -p m16 -P /dev/ttyACM0

HFUSE = 0xD9
LFUSE = 0xE4


OBJECTS = main.o twi_func.o

%.o: %.c
	$(COMPILE) -Wall -g -c $<

blob.elf: $(OBJECTS)
	$(COMPILE) -o blob.elf $(OBJECTS)

blob.hex: blob.elf
	rm -f blob.hex blob.eep.hex
	avr-objcopy -j .text -j .data -O ihex blob.elf blob.hex
	avr-size blob.hex

flash: blob.hex
	$(AVRDUDE) -U flash:w:blob.hex:i

fuses:
	$(AVRDUDE) -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

all: blob.hex
	
clean:
	rm -f $(OBJECTS)
	rm -f blob.elf blob.hex
