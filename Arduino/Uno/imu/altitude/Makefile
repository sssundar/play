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
SOURCES += event_queue.c
SOURCES += timer_driver.c

all: test-timer_driver 

test-timer_driver:		
	python config.py gcc > ./inc/config.h
	gcc $(FLAGS) -g test_timer_driver.c $(SOURCES) -o test_timer_driver
	#./test_timer_driver 

clean:		
	rm -f ./inc/config.h
	rm -f ./test_timer_driver	

.PHONY: clean all