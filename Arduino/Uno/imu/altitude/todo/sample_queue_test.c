/**
  * Tests the queue (can_)(en|de)queue functions
  * without interrupts, just for naive correctness.
  *
  * @title sample_queue_test.c 
  * @author sushant sundaresh
  * @lastrevised 28 November 2016
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h> 
#include <assert.h>

#include "bool.h"
#include "data.h"
#include "sample_queue.h"

int main (void) {
	/* 	Initialize State */	
	struct sample_queue q;
	sample_queue_init(&q);
	
	struct data record;
	struct data dump;	
	uint8_t i; 

	/* Queue is empty - can we enqueue? can we dequeue? */
	assert (sample_queue_can_enqueue(&q) == ktrue);
	assert (sample_queue_can_dequeue(&q) == kfalse);

	/* Create a mock data sample */
	set_sample_type(&record, (uint8_t) kbmp180_barometry);	
	struct sample *record_sample = &(record.s);
	for (i = 0; i < MAX_DATA_BYTES; i++) {
		record_sample->bytes[0] = 0x80 + i;		
	}	
	set_sample_bytes(&record, record_sample);

	/* Enqueue, then dequeue, and verify the two records are the same */
	sample_queue_enqueue(&record, &q);
	sample_queue_dequeue(&dump, &q);
	
	assert(q.tail == 1);

	assert (dump.type == record.type);
	for (i = 0; i < MAX_DATA_BYTES; i++) {
		assert(dump.s.bytes[i] == record.s.bytes[i]);	
	}

	/* Test full condition with a bit of wraparound */
	do {
		sample_queue_enqueue(&record, &q);
	} while (sample_queue_can_enqueue(&q) == ktrue);
	
	sample_queue_dequeue(&dump, &q);
	sample_queue_dequeue(&dump, &q);
	
	do {
		sample_queue_enqueue(&record,&q);
	} while (sample_queue_can_enqueue(&q) == ktrue);
	
	assert (q.tail == 2);
	assert (q.head == 3);

	/* Test empty condition with a bit of wraparound */
	do {
		sample_queue_dequeue(&dump, &q);
	} while (sample_queue_can_dequeue(&q) == ktrue);

	assert (q.tail == q.head);
	assert (q.head == 2);

	printf("All tests passed.\n");
	exit(0);
}