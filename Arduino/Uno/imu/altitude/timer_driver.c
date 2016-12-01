#include "config.h"
#include "io.h"
#include "interrupts.h"

#include "timer_driver.h"

static void timer_notify(sTimerDriver *self, sEventQueue *client, uint8_t ticks) {

}

static void timer_update_wait_counters(sTimerDriver *self) {

}

static void timer_increment_ticks(sTimerDriver *self) {

}                    

eStatus timer_init(sTimerDriver *timer, sTimerClient *clients, uint8_t max_clients) {
    PROTECT(); uint8_t flags = SREG; cli();
    if ((self == NULL) || ((clients == NULL) && (max_clients != 0))) { return kerror; }
    
    self->clients = clients;    
    self->max_clients = max_clients;
    self->num_clients = 0;
    
    sTicks *ticks = &(self->ticks);
    int k;
    for (k = 0; k < CLOCK_BYTE_WIDTH; k++) {
        ticks->count[k] = 0;
    }
    UNPROTECT(); SREG = flags; // Restore interrupt state
}

void timer_deinit(sTimerDriver *timer);                                      

}                                     

eStatus timer_register(sTimerDriver *timer, sEventQueue *client, uint8_t ticks) {
    return kfailure;
}

void timer_get_ticks (sTimerDriver *timer, sTicks *ticks) {

}

static void clock_init (void) {
}

static void clock_increment (void) {
    int k;  
    for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {
        global_clock.count[k]++;
        if ( !(global_clock_count[k] == 0) ) {      
            break;
        }
    }
}

static void clock_get_ticks (struct ticks *dst) {  
    cli();
    int k;
    for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {
        dst->count[k] = global_clock.count[k];
    }
    sei();
}
