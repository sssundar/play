#include "types.h"
#include "io.h"
#include "interrupts.h"
#include "timer_driver.h"

static void timer_notify(sTimerDriver *self, sEventQueue *client, uint8_t ticks) {

}

static void timer_update_wait_counters(sTimerDriver *self) {

}

static void timer_increment_ticks(sTimerDriver *self) {

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

eStatus timer_init(sTimerDriver *timer, sTimerClient *clients, uint8_t max_clients) {    
    if ((timer == NULL) || ((clients == NULL) && (max_clients != 0))) { return kerror; }
    
    _PROTECT(kinterrupt_save_flags);     
    sTicks *ticks = &(timer->ticks);
    int k;
    for (k = 0; k < CLOCK_BYTE_WIDTH; k++) {
        ticks->count[k] = 0;
    }

    timer->max_clients = max_clients;
    timer->num_clients = 0;
    timer->clients = clients;       

    /* Set up 199.8 Hz interrupt on Timer0 */
    TCCR0A = 0x02;                      // No compare outputs enabled on IO pins
                                        // Clear on compare 
    TCCR0B = 0x05;                      // 1024-prescaler off 16MHz base clock  
    OCR0A = 0x52;                       // Compare at 82 
    TIMSK0 = 0x02;                      // Interrupt at comparison equality         

    _RELEASE(kinterrupt_restore_flags);
    return ksuccess;
}

void timer_deinit(sTimerDriver *timer) {
    if (timer == NULL) { return; }
    _PROTECT(kinterrupt_save_flags);
    timer->max_clients = 0;        
    timer->num_clients = 0;
    timer->clients = NULL;
    TIMSK0 = 0x00;                  // Do not interrupt at comparison equality
    _RELEASE(kinterrupt_restore_flags);    
}                                     

eStatus timer_register(sTimerDriver *timer, sEventQueue *client, uint8_t ticks) {
    return kfailure;
}

eStatus timer_get_ticks (sTimerDriver *timer, sTicks *ticks) {

}
