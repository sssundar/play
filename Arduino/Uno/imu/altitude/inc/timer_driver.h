/**
  * @file timer_driver.h
  * @author Sushant Sundaresh
  * @date 2016-11-26
  * @details
  * A partial hardware driver for the Atmega 328p timers. 
  */ 

#ifndef _TIMER_DRIVER_H_
#define _TIMER_DRIVER_H_

#include "types.h"
#include "event_queue.h"

/**
  * @details
  * An extended unsigned integer of CLOCK_BYTE_WIDTH bytes. Intended to increase
  * how long we can track timer ticks without a timer overflow. 
  */
#define CLOCK_BYTE_WIDTH 3
typedef struct sTicks {  
    uint8_t count[CLOCK_BYTE_WIDTH]; // Max index is the least significant byte
} sTicks;

/**
  * @details
  * Contains [ticks per cycle, ticks left this cycle, sEventQueue to notify] 
  * The idea is, a client of this timer wants to be notified every X timer ticks.
  * The client registers an sEventQueue and the timer driver owning this array
  * manages the rest.
  */
#define NUM_TIMER_CLIENTS 3
typedef struct sTimerClient {
    uint8_t ticks_per_notification;         // constant once initialized
    uint8_t ticks_left_till_notification;   // counts down
    sEventQueue client;                     // who to notify
} sTimerClient;

/**
  * @details 
  * A Timer Driver object 
  */
typedef struct sTimerDriver {
    sTicks ticks;               // Counts up to 2.9 hours at 
                                //  100 Hz counting up a 24 bit sTicks.   
    uint8_t max_clients;        // How many clients this timer supports.
    uint8_t num_clients;        // How many clients are registered
    sTimerClient *clients;      // num_clients length array of clients
                                //  clients will be notified in the order of
                                //  registration
} sTimerDriver;

/** 
  * @details
  * This function saves the interrupt state then disables interrupts. 
  * It sets up a 99.9 Hz timer driver object controlling Timer 0, then
  * restores the old interrupt state.
  *
  * @param [in] self an uninitialized timer driver object  
  * @param [in] clients a pointer to a sTimerClient array of length num_clients
  * @param [in] max_clients the size of clients
  */
void timer_init(sTimerDriver *self, sTimerClient *clients, uint8_t max_clients);

/**
  * @details
  * This function saves the interrupt state then disables interrupts. 
  * It disables Timer 0, then restores the interrupt state.
  *
  * @param [in] self an initialized timer driver object  
  */
void timer_deinit(sTimerDriver *self);                                      

/**
  * @details
  * This function saves the interrupt state then disables interrupts. 
  * It registers an event queue to the timer to be notified every
  * ticks ticks with [type: kevent_timer, data: ticks]. It then
  * restores interrupt state.
  * 
  * @param [in] self an initialized timer driver object
  * @param [in] client an event queue object to notify every ticks ticks
  * @param [in] ticks the period of notification in base clock ticks
  * @return #ksuccess: successfully registered client queue
  * @return #kerror: self or client is null, or ticks is 0.
  * @return #kfailure: client registry is full. you were not registered.
  */
eStatus timer_register(sTimerDriver *self, sEventQueue *client, uint8_t ticks);  

/** 
  * @details
  * This function saves the interrupt state and then disables interrupts.
  * It copies the current timer tick from the timer driver object specified
  * over to the destination structure, then restores the interrupt context.
  *
  * @param [in] dest a pointer to a pre-allocated sTicks structure
  *              that will be set to the current global clock tick
  */
void timer_get_ticks (sTimerDriver *self, sTicks *dest); 

#endif 
// _TIMER_DRIVER_H_