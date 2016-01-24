all: code test load view

code:
	avr-gcc -g -mmcu=atmega328p -c come_down_from_there.c
	avr-gcc -g -mmcu=atmega328p -o come_down_from_there.elf come_down_from_there.o

test: 
	gcc -g test_queue_accessors.c -o test_queue

load:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:come_down_from_there.elf

view:
	python hear_altitude.py

clean:	
	rm -f come_down_from_there.o come_down_from_there.elf 
	rm -f test_queue

.PHONY: clean all