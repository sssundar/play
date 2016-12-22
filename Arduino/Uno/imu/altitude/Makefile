# @file Makefile
# @details Altitude sampling build script
# @author Sushant Sundaresh
# @date 2016-11-30

HEADERS += 
HEADERS += -I./inc/

FLAGS = 
FLAGS += -Wall
FLAGS += -Werror
FLAGS += -Wextra
FLAGS += -std=c99
FLAGS += $(HEADERS)

SOURCES =
SOURCES += interrupts.c
SOURCES += timer_driver.c
SOURCES += event_queue.c
SOURCES += data.c
SOURCES += sample_queue.c
SOURCES += uart_driver.c

all: clean

test: clean test-timer_driver_sw test-event_queue_sw test-sample_queue_sw test-uart_driver_sw

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
	gcc $(FLAGS) Wl,--wrap=eventq_enqueue -Wl,--wrap=uart_byte_buffer_receive_ -Wl,--wrap=uart_byte_buffer_transmit_ -g test_uart_driver.c test_helpers.c $(SOURCES) -o test_uart_driver_sw
	./test_uart_driver_sw

clean:		
	rm -f ./inc/config.h
	rm -f ./test_timer_driver_sw	
	rm -f ./test_event_queue_sw
	rm -f ./test_sample_queue_sw
	rm -f ./test_uart_driver_sw

.PHONY: clean all test