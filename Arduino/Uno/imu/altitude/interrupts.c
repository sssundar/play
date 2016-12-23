#include "interrupts.h"

#if AVR
static eStatus mcu_flags_saved;
static uint8_t mcu_flags;
#endif

void _PROTECT(eInterruptRequest save_flags) {
#if AVR
    if (save_flags == kinterrupt_save_flags) { 
        mcu_flags = SREG; 
        mcu_flags_saved = ktrue; 
    }
    cli();
#else
    (void) save_flags;
#endif
}

void _RELEASE(eInterruptRequest restore_flags) {
#if AVR
    if ((restore_flags == kinterrupt_restore_flags) && (mcu_flags_saved == ktrue)) {
        SREG = mcu_flags;
    } else if (restore_flags == kinterrupt_do_not_restore_flags) {
        sei();
    }   
#else
    (void) restore_flags;
#endif
}

void _ENABLE_SLEEP(void) {
#if AVR
    sleep_enable(); 
#endif 
}

void _SLEEP(void) {
#if AVR    
    sei();
    sleep_cpu();
#endif 
}
