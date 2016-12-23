# @file Makefile
# @details Altitude sampling build script
# @todo Review http://www.avrfreaks.net/forum/how-optimize-size if code size is problematic.
# @author Sushant Sundaresh
# @date 2016-11-30

HEADERS += 
HEADERS += -I./inc/

FLAGS = 
FLAGS += -Wall
FLAGS += -Wundef
FLAGS += -Werror
FLAGS += -Wextra
FLAGS += -std=c99
FLAGS += -Os
FLAGS += -fno-inline-small-functions
FLAGS += -ffunction-sections
FLAGS += -fdata-sections 
FLAGS += -fshort-enums
FLAGS += $(HEADERS)

LINKFLAGS =
LINKFLAGS += -Wl,-Map=$(TARGET).map,--cref ### WHAT IS THIS??? 
LINKFLAGS += -Wl,-gc-sections
LINKFLAGS += -Wl,--relax

SOURCES =
SOURCES += interrupts.c
SOURCES += timer_driver.c
SOURCES += event_queue.c
SOURCES += data.c
SOURCES += sample_queue.c
SOURCES += uart_hal.c
SOURCES += uart_driver.c
SOURCES += twi_driver.c

OBJS = $(SOURCES:.c=.o)

.PHONY:
all: clean

.PHONY:
test_sw: clean test-timer_driver_sw test-event_queue_sw test-sample_queue_sw test-uart_driver_sw

test-timer_driver_sw: 
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -Wl,--wrap=eventq_enqueue -g test_timer_driver.c test_helpers.c $(SOURCES) -o test_timer_driver_sw
	./test_timer_driver_sw

test-event_queue_sw:
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -g test_event_queue.c test_helpers.c $(SOURCES) -o test_event_queue_sw
	./test_event_queue_sw

test-sample_queue_sw:
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -g test_sample_queue.c test_helpers.c $(SOURCES) -o test_sample_queue_sw
	./test_sample_queue_sw

test-uart_driver_sw:
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -Wl,--wrap=eventq_enqueue -Wl,--wrap=uart_byte_buffer_receive_ -Wl,--wrap=uart_byte_buffer_transmit_ -g test_uart_driver.c test_helpers.c $(SOURCES) -o test_uart_driver_sw
	./test_uart_driver_sw

.PHONY:
test-timer_hw:
	python config.py avr > ./inc/config.h	 	
	avr-gcc $(FLAGS) -Wno-main $(LINKFLAGS) -g -mmcu=atmega328p test_timer_hw.c interrupts.c timer_driver.c event_queue.c -o test_timer_hw.elf	

.PHONY:
load-timer_hw:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:test_timer_hw.elf

.PHONY:
clean:		
	rm -f ./inc/config.h
	rm -f ./test_timer_driver_sw	
	rm -f ./test_event_queue_sw
	rm -f ./test_sample_queue_sw
	rm -f ./test_uart_driver_sw
	rm -f ./*.S
	rm -f ./*.o
	rm -f ./*.elf
