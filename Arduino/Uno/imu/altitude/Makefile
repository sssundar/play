all: code test load view

code:
	avr-gcc -g -mmcu=atmega328p -c come_down_from_there.c
	avr-gcc -g -mmcu=atmega328p -o come_down_from_there.elf come_down_from_there.o

test: 
	gcc -g sample_queue_test.c sample_queue.c data.c clock.c -o sample_queue_test

load:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:come_down_from_there.elf

view:
	python hear_altitude.py

clean:	
	rm -f come_down_from_there.o come_down_from_there.elf 
	rm -f sample_queue_test

.PHONY: clean all