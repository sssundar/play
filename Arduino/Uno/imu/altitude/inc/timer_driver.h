/**
  * @file timer_driver.h
  * @author Sushant Sundaresh
  * @date 2016-11-26
  * @details
  * A partial hardware driver for the Atmega 328p timers. 
  */ 

#ifndef _TIMER_DRIVER_H_
#define _TIMER_DRIVER_H_

#include "config.h"
#include "io.h"
#include "interrupts.h"
#include "types.h"
#include "stdmath.h"
#include "event_queue.h"

#if AVR
/** 
  * @details 
  * Global statically allocated timer0 driver object that must be
  * allocated and initialized by the user of this driver.
  */
extern sTimerDriver timer0;
#endif

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
#define NUM_TIMER_CLIENTS 2
typedef struct sTimerClient {
    uint8_t ticks_per_notification;         // constant once initialized
    uint8_t ticks_left_till_notification;   // counts down
    sEventQueue *client;                    // who to notify
} sTimerClient;

/**
  * @details 
  * A Timer Driver object 
  */
typedef struct sTimerDriver {    
    sTicks ticks;               // Counts up to 1.45 hours at 
                                //  200 Hz counting up a 24 bit sTicks.   
    uint8_t max_clients;        // How many clients this timer supports.
    uint8_t num_clients;        // How many clients are registered
    sTimerClient *clients;      // num_clients length array of clients
                                //  clients will be notified in the order of
                                //  registration
} sTimerDriver;

#if AVR == 0
/**
  * @details
  * Mocked interrupt service routine for the timer, Timer 0, controlled by this driver
  *
  * @param [in] timer an initialized timer driver object
  */
void ISR_TIMER(sTimerDriver *timer);
#endif 

/** 
  * @details
  * This function saves the interrupt state then disables interrupts. 
  * It sets up a 199.8 Hz timer driver object controlling Timer 0 
  * (16 MHz / (1024 Prescaler * 82) = 199.8 Hz). It then
  * restores interrupt state.
  *
  * @param [in] timer an uninitialized timer driver object  
  * @param [in] clients a pointer to a sTimerClient array of length num_clients
  * @param [in] max_clients the size of clients
  * @return #kerror: One or more of the pointers input were invalid.
  * @return #ksuccess: Successfully initialized timer driver.
  */
eStatus timer_init(sTimerDriver *timer, sTimerClient *clients, uint8_t max_clients);

/**
  * @details
  * This function saves the interrupt state then disables interrupts. 
  * It disables Timer 0. It then restores interrupt state.
  *
  * @param [in] timer an initialized timer driver object  
  */
void timer_deinit(sTimerDriver *timer);                                      

/**
  * @details
  * This function saves the interrupt state then disables interrupts. 
  * It registers an event queue to the timer to be notified every
  * ticks ticks with [eEvent: ktimer_event, data: ticks]. It then
  * restores interrupt state.
  * 
  * @param [in] timer an initialized timer driver object
  * @param [in] client an event queue to be notified 
  * @param [in] ticks the period of notification in clock ticks for this timer
  * @return #ksuccess: Client queue successfully registered.
  * @return #kerror: Client queue or timer driver is null.
  * @return #kfailure: Out of memory in client registry. Client not registered.
  */
eStatus timer_register(sTimerDriver *timer, sEventQueue *client, uint8_t ticks);  

/**
  * @details
  * This function saves the interrupt state then disables interrupts.
  * It retrieves the specified timer's tick count, which can be easily
  * multiplied by its true period to get a physical time. It then
  * restores interrupt state.
  *
  * @param [in] timer an initialized timer driver object
  * @param [in] ticks the destination ticks structure to write to
  * @return #kerror: timer or ticks are null.
  * @return #ksuccess: successfully copied over current tick count
  */
eStatus timer_get_ticks (sTimerDriver *timer, sTicks *ticks); 

#endif 
// _TIMER_DRIVER_H_
