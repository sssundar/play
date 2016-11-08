/**  
  * @contents Extended clock
  * @author Sushant Sundaresh
  * @lastrevised 22 March 2016
  */

#include "clock.h"
#include <avr/interrupt.h>

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
