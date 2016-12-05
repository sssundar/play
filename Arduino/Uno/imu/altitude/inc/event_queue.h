/**
  * @file event_queue.h
  * @author Sushant Sundaresh
  * @date 2016-12-03
  * @details 
  * A circular queue object of fixed length, intended to buffer system events
  * The length is fixed at EVENT_QUEUE_LENGTH exluding the one dead data element
  * when the queue is full. 
  */
  
#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include "config.h"
#include "types.h"
#include "interrupts.h"
#include "stdmath.h"

/** 
  * @details 
  * Supported event types 
  */
typedef enum eEventType {
    kevent_timer,       // a timer notification event 
    kevent_serial_rx    // a serial packet received event
} eEventType;

/** 
  * @details 
  * An event consists of a type and a byte of data
  */
typedef struct sEvent {
    eEventType type;
    uint8_t data;
} sEvent;

/**
  * @details
  * A event queue object  
  */
#define EVENT_QUEUE_LENGTH 31   // Must be 2^n - 1
typedef struct sEventQueue {
    sEvent buffer[EVENT_QUEUE_LENGTH+1];    
    uint8_t tail;
    uint8_t head;    
} sEventQueue; 

/** 
  * @details
  * Protects itself from interrupts then initializes the queue in question.
  * @param [in] queue an uninitialized preallocated event queue 
  * @return #kerror: invalid pointer
  * @return #ksuccess: event queue object initialized
  */
eStatus eventq_init(sEventQueue *queue);

/** 
  * @details
  * Protects itself from interrupts then deinitializes the queue in question.
  * Deinitializes the queue in question. Does not deallocate anything.  
  * @param [in] queue an initialized event queue 
  */
void eventq_deinit(sEventQueue *queue);

/** 
  * @details
  * Protects itself from interrupts then checks whether the given event queue 
  * has room for one more event, and enqueues the event if possible.  
  * @param [in] queue an initialized event queue 
  * @param [in] event the event to enqueue
  * @return #kerror: either pointer is null
  * @return #kfailure: no room in the queue
  * @return #ksuccess: event successfully enqueued in queue
  */
eStatus eventq_enqueue(sEventQueue* restrict queue, const sEvent* restrict event);

/** 
  * @details
  * Protects itself from interrupts then checks whether the given event queue 
  * has an event to dequeue, then dequeues the event.
  * @param [in] queue an initialized event queue 
  * @param [in] event the place the store the dequeued event
  * @return #kerror: either pointer is null
  * @return #kfailure: no events in the queue
  * @return #ksuccess: event successfully dequeued
  */
eStatus eventq_dequeue(sEventQueue* restrict queue, sEvent* restrict event);    

#endif 
// _EVENT_QUEUE_H