# Makefile for Altitude Sampling
# @author Sushant Sundaresh
# @date 2016-11-27

HEADERS += 
HEADERS += -I./inc/
HEADERS += -I./stubs/

FLAGS = 
FLAGS += -Wall
FLAGS += -std=c99
FLAGS += $(HEADERS)

SOURCES =
SOURCES += ./src/c/timer_driver.c

TEST_CONFIG =
TEST_CONFIG += //Automatically Generated. Changes will be overwritten.\n
TEST_CONFIG += #ifndef _CONFIG_H_\n
TEST_CONFIG += #define _CONFIG_H_\n
TEST_CONFIG += #define AVR 0
TEST_CONFIG += #endif\n
TEST_CONFIG += // _CONFIG_H_ 

BUILD_CONFIG =
BUILD_CONFIG += //Automatically Generated. Changes will be overwritten.\n
BUILD_CONFIG += #ifndef _CONFIG_H_\n
BUILD_CONFIG += #define _CONFIG_H_\n 
BUILD_CONFIG += #define AVR 1
BUILD_CONFIG += #endif\n 
BUILD_CONFIG += // _CONFIG_H_

test-timer_driver:		
	echo  $(TEST_CONFIG) > ./inc/config.h
	gcc FLAGS -g ./test/test_timer_driver.c $(SOURCES) -o ./test/test_timer_driver
	./test/test_timer_driver 

clean:	
	rm -f ./inc/config.h
	rm -f ./test/test_timer_driver
	rm -f ./src/c/*.o

.PHONY: clean