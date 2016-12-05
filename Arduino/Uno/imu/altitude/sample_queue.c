#include "sample_queue.h"

eStatus sampleq_init(sSampleQueue *queue) {
    if (queue == NULL) { return kerror; }    
    queue->tail = 0;
    queue->head = 0;    
    return ksuccess;
}

void sampleq_deinit(sSampleQueue *queue) {
    (void) queue;
    return;
}

eStatus sampleq_enqueue(sSampleQueue* restrict queue, const sData* restrict data) {
    if ((queue == NULL) || (data == NULL)) { return kerror; }
    eStatus status = kfailure;
    uint8_t next_tail = (queue->tail + 1) & SAMPLE_QUEUE_LENGTH;
    if (next_tail != queue->head) {
        sData *dst = &(queue->buffer[queue->tail]);                                 
        data_copy(data, dst);
        queue->tail = next_tail;        
        status = ksuccess;   
    }    
    return status;
}

eStatus sampleq_dequeue(sSampleQueue* restrict queue, sData* restrict data) {
    if ((queue == NULL) || (data == NULL)) { return kerror; }
    eStatus status = kfailure;
    if (queue->tail != queue->head) {
        sData *src = &(queue->buffer[queue->head]);
        data_copy(src, data);
        queue->head = (queue->head + 1) & SAMPLE_QUEUE_LENGTH;
        status = ksuccess;
    }    
    return status;
}
