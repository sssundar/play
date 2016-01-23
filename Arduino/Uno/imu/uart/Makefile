all: speak load_speak listen


speak:
	avra we_need_to_talk.s

load_speak: 
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:we_need_to_talk.s.hex

listen:
	python hear_uno.py


clean:	
	rm -f we_need_to_talk.s.* 	

.PHONY: clean all