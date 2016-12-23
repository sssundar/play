/**
  * @file test_timer_hw.c
  * @author Sushant Sundaresh
  * @date 2016-12-22
  * @details 
  * Tests the workflow with a simple main loop that just instantiates a timer object and event queue, 
  *  creates a tick notification at 1 Hz, and toggles a digital pin driving an LED as a result of timer events.  
  */    
  
#include "timer_driver.h"
#include "event_queue.h"

#define HZ_1 200    // 200 tick period for 1 Hz events off a 200 Hz timer
sTimerDriver timer0;
sTimerClient clients[1];
sEventQueue inbox;

void main(void) __attribute__((noreturn));
void main(void) {     
    _ENABLE_SLEEP();

    /*  Set up Digital Pin 6 (Uno R3), i.e. PD6, as an output, no pull-up,
        so it can be used for LED-debugging */
    DDRD |= 0x40;                       // PD6 is an output 
    PORTD &= 0xBF;                      // Clear PD6's output
    // To make PD6 toggle for debugging: use PIND |= 0x40;  
    // and place a 1kOhm + LED off Digital Pin 6 with a loop to GND

    eventq_init(&inbox);    
    timer_init(&timer0, (sTimerClient *) clients, 1);    
    
    timer_register(&timer0, &inbox, HZ_1);  
    sEvent event;    

    while (1) {
        while (ksuccess != eventq_dequeue(&inbox, &event)) {
            _SLEEP();
        } 
        if ((event.type == kevent_timer) && (event.data == HZ_1)) {
            PIND |= 0x40;  
        }
    }
}
