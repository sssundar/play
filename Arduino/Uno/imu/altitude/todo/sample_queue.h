#ifndef SAMPLE_QUEUE
#define SAMPLE_QUEUE

#include "bool.h"
#include "data.h"

/* 	circular queue 
	length is fixed at ~1 kilobyte, half our SRAM 	

	tail == head => empty
	head == tail + 1 % QUEUE_LENGTH => full 
	enqueuing: tail += 1 % QUEUE_LENGTH, head = head, post increment
	dequeuing: head += 1 % QUEUE_LENGTH, tail = tail; post decrement
	
	has at least 1 dead data element when 'full.'
*/

// Must be 2^n - 1
#define QUEUE_LENGTH 127

struct sample_queue {
	uint8_t tail;
	uint8_t head;
	struct data buffer[QUEUE_LENGTH+1];
};

/* 	None of the following are interrupt safe, because for this project,
	they don't need to be */

void sample_queue_init (struct sample_queue *q);
BOOL sample_queue_can_enqueue (struct sample_queue* q);
BOOL sample_queue_can_dequeue (struct sample_queue* q);
void sample_queue_enqueue (struct data* d, struct sample_queue* q); /* must call can_enqueue first */
void sample_queue_dequeue (struct data* d, struct sample_queue* q); /* must call can_dequeue first */

#endif 
// SAMPLE_QUEUE