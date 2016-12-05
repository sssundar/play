/**
  * @file sample_queue.h
  * @author Sushant Sundaresh
  * @date 2016-12-04
  * @details 
  * A circular queue object of fixed length, intended to buffer data samples
  * The length is fixed at EVENT_QUEUE_LENGTH exluding the one dead data element
  * when the queue is full. This queue is only intended to be accessed from
  * a non-interrupt context.
  */
  
#ifndef _SAMPLE_QUEUE_H_
#define _SAMPLE_QUEUE_H_

#include "config.h"
#include "types.h"
#include "interrupts.h"
#include "stdmath.h"
#include "data.h"

/**
  * @details
  * A sample queue object  
  */
#define SAMPLE_QUEUE_LENGTH 31   // Must be 2^n - 1
typedef struct sSampleQueue {
    sData buffer[SAMPLE_QUEUE_LENGTH+1];    
    uint8_t tail;
    uint8_t head;    
} sSampleQueue; 

/** 
  * @details
  * Initializes the queue in question.
  * @param [in] queue an uninitialized preallocated sample queue 
  * @return #kerror: invalid pointer
  * @return #ksuccess: sample queue object initialized
  */
eStatus sampleq_init(sSampleQueue *queue);

/** 
  * @details
  * Deinitializes the queue in question. Does not deallocate anything.  
  * @param [in] queue an initialized sample queue 
  */
void sampleq_deinit(sSampleQueue *queue);

/** 
  * @details
  * Checks whether the given sample queue has room for one more sample
  * and enqueues the sample if possible.  
  * @todo verify the data input has a valid type
  * @param [in] queue an initialized event queue 
  * @param [in] data the sample to enqueue
  * @return #kerror: either pointer is null
  * @return #kfailure: no room in the queue
  * @return #ksuccess: sample successfully enqueued in queue
  */
eStatus sampleq_enqueue(sSampleQueue* restrict queue, const sData* restrict data);

/** 
  * @details
  * Checks whether the given sample queue has a sample to dequeue, 
  * then dequeues the sample.
  * @param [in] queue an initialized event queue 
  * @param [in] data where to copy the dequeued sample
  * @return #kerror: either pointer is null
  * @return #kfailure: no data in the queue
  * @return #ksuccess: data successfully dequeued
  */
eStatus sampleq_dequeue(sSampleQueue* restrict queue, sData* restrict data);    

#endif 
// _SAMPLE_QUEUE_H_
