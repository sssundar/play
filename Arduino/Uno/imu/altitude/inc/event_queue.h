#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include "types.h"
#include <math.h>  
#include "interrupts.h"

/** 
  * @details 
  * Supported event types 
  */
typedef enum eEventType {
	kevent_timer, 			// a timer notification event 
	kevent_serial_rx 		// a serial packet received event
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
  * TODO Implement
  */
typedef struct sEventQueue {
	uint8_t ignored;
} sEventQueue; 

/** 
  * @details
  * Checks whether the given event queue has room for one more event,
  * and enqueues the event if possible.
  * 
  * @param [in] queue an initialized event queue 
  * @param [in] event the event to enqueue
  * @return #kerror: either pointer is null
  * @return #kfailure: no room in the queue
  * @return #ksuccess: event successfully enqueued in queue
  */
eStatus eventq_enqueue (sEventQueue *queue, const sEvent *event);

#endif 
// _EVENT_QUEUE_H