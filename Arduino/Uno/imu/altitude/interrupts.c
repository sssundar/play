#include "config.h"

#if AVR
#include <avr/io.h> 
#include <avr/interrupt.h>
#endif

static uint8_t mcu_flags;

void _PROTECT(bool will_restore) {
#if AVR
	mcu_flags = SREG; 
	cli();
#endif
	
}

void _RELEASE(bool restore) {
	SREG = mcu_flags;
	sei();
}
