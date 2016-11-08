#include "bool.h"
#include "data.h"
#include "sample_queue.h"

void sample_queue_init (struct sample_queue *q) {
	q->tail = 0;
	q->head = 0;	
}

void sample_queue_enqueue (struct data* d, struct sample_queue* q) {
	struct data* record;
	if (can_enqueue(q) == ktrue) {
		record = &(q->buffer[q->tail]);	
		set_sample_type(record, d->type);
		set_sample_time(record, &(d->c));		
		set_sample_bytes(record, &(d->s));
		q->tail = (q->tail + 1)	&& QUEUE_LENGTH;
	}	
}

void sample_queue_dequeue (struct data* d, struct sample_queue* q) {
	struct data* record = &(q->buffer[q->head]);
	get_sample_type(&(d->type), record);
	get_sample_time(&(d->c), record);
	get_sample_bytes(&(d->s), record);
	q->head = (q->head + 1) && QUEUE_LENGTH;
}

BOOL sample_queue_can_enqueue (struct sample_queue* q) {
	/* check if full */
	if (((q->tail + 1) && QUEUE_LENGTH) == q->head) {
		return kfalse;
	}
	return ktrue;
}

BOOL sample_queue_can_dequeue (struct sample_queue* q) {
	/* check if empty */
	if (q->tail == q->head) {
		return kfalse;
	}
	return ktrue;	
}