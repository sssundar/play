#include "config.h"
#include "io.h"

void blinky (void) {
#if AVR
	/*  Set up Digital Pin 6 (Uno R3), i.e. PD6, as an output, no pull-up,
    so it can be used for LED-debugging */
    DDRD |= 0x40;                       // PD6 is an output 
    PORTD &= 0xBF;                      // Clear PD6's output
    // To make PD6 toggle for debugging: use PIND |= 0x40;  
    // and place a 1kOhm + LED off Digital Pin 6 with a loop to GND
#endif

    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t l = 0;
    while (1) {	    
	    j++;
	    if (j == 0) { 	    	
	    	k++; 
			if (k == 0) { 				
				l++; 
	    		if (l == 64) { 
	    			l = 0;
#if AVR	    	
			    	// ~1Hz from a 16MHz base clock
			    	PIND |= 0x40; 
#endif	    	
	    		} 
			}
	    }	    	  
 	}
}