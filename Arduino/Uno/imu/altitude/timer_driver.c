/**
  * @file timer_driver.c
  * @author Sushant Sundaresh
  * @date 2016-11-26
  * @details
  * A partial hardware driver for the Atmega 328p timers. 
  */ 

#include <avr/io.h> 
#include <avr/interrupt.h>
#include "timer_driver.h"

/* Global timer tick counter */
struct ticks global_clock;

void clock_init (void) {
	int k;
	for (k = 0; k < CLOCK_BYTE_WIDTH; k++) {
		global_clock.count[k] = 0;
	}
}

void clock_increment (void) {
	int k;	
	for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {
		global_clock.count[k]++;
		if ( !(global_clock_count[k] == 0) ) {		
			break;
		}
	}
}

void clock_get_ticks (struct ticks *dst) {	
	cli();
	int k;
	for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {
		dst->count[k] = global_clock.count[k];
	}
	sei();
}
