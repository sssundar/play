/**  
  * @contents Extended clock
  * @author Sushant Sundaresh
  * @lastrevised 22 March 2016
  */

#include "clock.h"
#include <avr/interrupt.h>

/* Global timer tick counter */
struct ticks global_clock;

/* A function to initialize the global clock. Not interrupt safe. */
void init_global_clock (void) {
	int k;
	for (k = 0; k < GLOBAL_CLOCK_BYTE_WIDTH; k++) {
		global_clock.count[k] = 0;
	}
}

/* Sets global ticks++ atomically, from within an interrupt context */
void increment_global_ticks (void) {
	int k;	
	for (k = GLOBAL_CLOCK_BYTE_WIDTH-1; k > -1; k--) {
		global_clock.count[k]++;
		if ( !(global_clock_count[k] == 0) ) {		
			break;
		}
	}
}

/* Get the global tick count atomically */
void get_global_ticks (struct ticks *dst) {	
	cli();
	int k;
	for (k = GLOBAL_CLOCK_BYTE_WIDTH-1; k > -1; k--) {
		dst->count[k] = global_clock.count[k];
	}
	sei();
}
