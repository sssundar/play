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

all: clean

test: clean test-timer_driver test-event_queue test-sample_queue

test-timer_driver: 
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -Wl,--wrap=eventq_enqueue -g test_timer_driver.c test_helpers.c $(SOURCES) -o test_timer_driver
	./test_timer_driver 

test-event_queue:
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -g test_event_queue.c test_helpers.c $(SOURCES) -o test_event_queue
	./test_event_queue

test-sample_queue:
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -g test_sample_queue.c test_helpers.c $(SOURCES) -o test_sample_queue
	./test_sample_queue

clean:		
	rm -f ./inc/config.h
	rm -f ./test_timer_driver	
	rm -f ./test_event_queue
	rm -f ./test_sample_queue

.PHONY: clean all test