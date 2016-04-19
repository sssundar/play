#ifndef SAMPLE_QUEUE
#define SAMPLE_QUEUE

#include "clock.h"

/* Data sample holder, for any data type */
struct sample {	
	uint8_t bytes[MAX_DATA_BYTES]; /* MAX_DATA_BYTES must be defined by user */
}

/* E.g. for sensor: BST-BMP180-DS000-09 on Adafruit p/n 1604 */
struct data {
	uint8_t type; 		/* Type of the data sample */	
	struct ticks c; 	/* Acquisition time, estimate */
	struct sample s; 	/* The sample itself */
}

/* 	circular queue 
	length is fixed at 256 4 byte structs, or 1 kilobyte, half our SRAM 
	naturally addition is modulu 256 for uint8_t tail/head indices

	tail == head => empty
	head == tail + 1 % 256 => full 
	enqueuing: tail += 1 % 256, head = head, post increment
	dequeuing: head += 1 % 256, tail = tail; post decrement
	notice, at least 1 dead data element even when full.
*/

#define QUEUE_LENGTH 255

struct circular_queue {
	uint8_t tail;
	uint8_t head;
	struct data buffer[QUEUE_LENGTH+1];
}

void set_sample_time(struct data* dst) {	
	get_global_ticks(&dst->c);
}

/* Queue accessor, enqueue. Not interrupt safe. */
bool enqueue (struct data* d, struct circular_queue* q) {
	struct data* record;
	if (can_enqueue(q) == IS_TRUE) {
		record = &(q->buffer[q->tail]);	
		record->type = d->type;
		set_data_ticks(record,d);
		record->ticks = d->ticks;
		record->MSB = d->MSB;
		record->LSB = d->LSB;
		record->XLSB = d->XLSB;
		q->tail += 1;
	}	
}

/* 	must call can_dequeue first 
	not interrupt safe */
void dequeue (struct data* d, struct circular_queue* q) {
	data* record = &(q->buffer[q->head]);
	sample->type = record->type;
	sample->bytes = record->bytes;
	
	sample->MSB = record->MSB;
	sample->LSB = record->LSB;
	sample->XLSB = record->XLSB;
	q->head += 1;

}

/* does not need to be interrupt safe - only called within handlers */
uint8_t can_enqueue (struct circular_queue* q) {
	/* check if full */
	if ((q->tail + 1) == q->head) {
		return (uint8_t) IS_FALSE;
	}
	return (uint8_t) IS_TRUE;
}

/* does not need to be interrupt safe - only called within handlers */
uint8_t can_dequeue	(struct circular_queue* q) {
	/* check if empty */
	if (q->tail == q->head) {
		return (uint8_t) IS_FALSE;
	}
	return (uint8_t) IS_TRUE;	
}

#endif /* queue.h */