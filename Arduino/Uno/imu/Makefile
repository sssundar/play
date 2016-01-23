all: blinkLED speak

blink:
	avra blinkLED.s	

load_blink: 
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:blinkLED.s.hex 



speak:
	avra we_need_to_talk.s

load_speak: 
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:we_need_to_talk.s.hex

listen:
	python hear_uno.py



altitude:
	avr-gcc -g -mmcu=atmega328p -c come_down_from_there.c
	avr-gcc -g -mmcu=atmega328p -o come_down_from_there.elf come_down_from_there.o

load_altitude:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:come_down_from_there.elf

measure:
	python hear_altitude.py



clean:
	rm -f blinkLED.s.* 
	rm -f we_need_to_talk.s.* 
	rm -f come_down_from_there.o come_down_from_there.elf

.PHONY: clean all