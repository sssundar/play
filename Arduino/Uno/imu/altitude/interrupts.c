#include "config.h"

#if AVR
#include "types.h"
#include <avr/io.h> 
#include <avr/interrupt.h>
#endif

static eStatus mcu_flags_saved;
static uint8_t mcu_flags;

void _PROTECT(eInterruptRequest save_flags) {
#if AVR
	if (save_flags == kinterrupt_save_flags) { 
		mcu_flags = SREG; 
		mcu_flags_saved = ktrue; 
	}
	cli();
#endif
}

void _RELEASE(eInterruptRequest restore_flags) {
#if AVR
	if ((restore_flags == kinterrupt_restore_flags) && (mcu_flags_saved == ktrue)) {
		SREG = mcu_flags;
	} else if (restore_flags == kinterrupt_do_not_restore_flags) {
		sei();
	}	
#endif
}
