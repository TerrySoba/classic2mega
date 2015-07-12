CC      = avr-gcc
CFLAGS  = -Wall -pedantic -Os -std=c99 -Werror
DEVICE  = atmega16
F_CPU   = 16000000	# in Hz 16000000
COMPILE = $(CC) $(CFLAGS) -DF_CPU=$(F_CPU) -mmcu=$(DEVICE)
AVRDUDE = avrdude -c stk500v2 -p m16 -P /dev/ttyACM0

HFUSE = 0x99
LFUSE = 0xCE


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
	
