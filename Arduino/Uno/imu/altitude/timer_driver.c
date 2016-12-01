#include "timer_driver.h"

/* Local Functions */

/**
  * @details
  * This function is called only by the timer interrupt service routine
  * and therefore is not critical code - it does not need to be protected from
  * interrupts. 
  * 
  * @param [in] client an event queue to notify with a kevent_timer message
  * @param [in] ticks the data for the kevent_timer message, i.e. the period being notified
  */
static void timer_notify(sEventQueue *client, uint8_t ticks) {
    sEvent event = { .type = kevent_timer, .data = ticks };
    eventq_enqueue(client, &event);
}

/**
  * @details
  * This function is called only by the timer interrupt service routine
  * and therefore is not critical code - it does not need to be protected from
  * interrupts. 
  * 
  * @param [in] timer an initialized timer driver object 
  */
static void timer_update_wait_counters(sTimerDriver *timer) {
    int8_t k;
    sTimerClient *c = timer->clients;
    for (k = 0; k < timer->num_clients; k++) {
        c.ticks_left_till_notification -= 1;
        if (c.ticks_left_till_notification == 0) {
            timer_notify(c->client, c.ticks_per_notification);
            c.ticks_left_till_notification = c.ticks_per_notification;
        }
        c++;
    }
}

/**
  * @details
  * This function is called only by the timer interrupt service routine
  * and therefore is not critical code - it does not need to be protected from
  * interrupts. 
  * 
  * @param [in] timer an initialized timer driver object 
  */
static void timer_increment_ticks(sTimerDriver *timer) {  
    int8_t k;  
    sTicks *t = &(timer->ticks);
    for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {
        t->count[k]++;
        if ( !(t->count[k] == 0) ) {      
            break;
        }
    }

    timer_update_wait_counters(timer);
}                    

/**
  * @details
  * Interrupt service routine for the timer, Timer 0, controlled by this driver
  */
#if AVR
ISR(TIMER0_COMPA_vect) {                    
    timer_increment_ticks(&timer0);
}
#else
void ISR_TIMER(sTimerDriver *timer) {
    timer_increment_ticks(&timer);
}
#endif

/* Public API */

eStatus timer_init(sTimerDriver *timer, sTimerClient *clients, uint8_t max_clients) {    
    if ((timer == NULL) || ((clients == NULL) && (max_clients != 0))) { return kerror; }
    
    _PROTECT(kinterrupt_save_flags);     
    sTicks *ticks = &(timer->ticks);
    int8_t k;
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
    if ((timer == NULL) || (client == NULL) || (ticks == 0)) { return kerror; }
    eStatus status = kfailure;
    _PROTECT(kinterrupt_save_flags);
    if (timer->num_clients < timer->max_clients) {        
        sTimerClient *c = timer->clients + timer->num_clients;
        c->ticks_per_notification = ticks;
        c->ticks_left_till_notification = ticks;
        c->client = client;
        timer->num_clients += 1;
        status = ksuccess;
    } 
    _RELEASE(kinterrupt_restore_flags);
    return status;
}

eStatus timer_get_ticks (sTimerDriver *timer, sTicks *ticks) {
    _PROTECT(kinterrupt_save_flags);    
    int8_t k;
    sTicks *t = &(timer->ticks);
    for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {
        ticks->count[k] = t->count[k];
    }    
    _RELEASE(kinterrupt_restore_flags);
}
