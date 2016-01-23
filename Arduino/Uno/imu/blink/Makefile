all: blinkLED

blink:
	avra blinkLED.s	

load_blink: 
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:blinkLED.s.hex 


clean:
	rm -f blinkLED.s.* 

.PHONY: clean all