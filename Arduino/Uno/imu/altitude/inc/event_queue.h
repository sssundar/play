#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

/* Supported event types */
typedef enum eEventType {
	kevent_timer,
	kevent_serial_rx
} eEventType;

typedef struct sEvent {
	eEventType type;
	uint8_t data;
} sEvent;

#endif 
// _EVENT_QUEUE_H