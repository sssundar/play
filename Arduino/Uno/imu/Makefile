all: blinkLED speak

blink:
	avra blinkLED.s	

speak:
	avra we_need_to_talk.s

listen:
	python hear_uno.py

load_blink: 
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:blinkLED.s.hex 

load_speak: 
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:we_need_to_talk.s.hex

clean:
	rm -f blinkLED.s.* we_need_to_talk.s.*

.PHONY: clean all