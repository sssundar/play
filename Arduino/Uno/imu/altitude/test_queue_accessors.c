/**
  * Tests the queue (can_)(en|de)queue functions in "come_down_from_there.h"
  * without interrupts, just for naive correctness.
  *
  * @title test_queue_accessors.c 
  * @author sushant sundaresh
  * @lastrevised 23 January 2016
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h> 
#include <assert.h>
#include "come_down_from_there.h"

int main (void) {
	/* 	Initialize and Clear Queue */
	circular_queue q;
	data* temp;
	data record;
	data dump;
	int i;

	q.tail = 0;
	q.head = 0;
	for (i = 0; i < QUEUE_LENGTH+1; i++) {
		temp = &(q.buffer[i]);
		temp->type = 0;
		temp->MSB = 0;
		temp->LSB = 0;
		temp->XLSB = 0;
	}

	/* Queue is empty - can we enqueue? can we dequeue? */
	assert (can_enqueue(&q) == TRU);
	assert (can_dequeue(&q) == FLS);

	/* Enqueue, then dequeue, and verify the two records are the same */
	record.type = BAROMETER_DATA; 
	record.MSB = 0xAB;
	record.LSB = 0xBA;
	record.XLSB = 0xCC;

	enqueue(&record, &q);
	dequeue(&dump, &q);
	
	assert (q.tail == 1);

	assert (dump.type == record.type);
	assert (dump.MSB == record.MSB);
	assert (dump.LSB == record.LSB);
	assert (dump.XLSB == record.XLSB);

	/* Test full condition with a bit of wraparound */
	do {
		enqueue(&record,&q);
	} while (can_enqueue(&q) == TRU);
	
	dequeue(&dump, &q);
	dequeue(&dump, &q);
	
	do {
		enqueue(&record,&q);
	} while (can_enqueue(&q) == TRU);
	
	assert (q.tail == 2);
	assert (q.head == 3);

	/* Test empty condition with a bit of wraparound */
	do {
		dequeue(&dump, &q);
	} while (can_dequeue(&q) == TRU);

	assert (q.tail == q.head);
	assert (q.head == 2);

	printf("All tests passed.\n");
	exit(0);
}