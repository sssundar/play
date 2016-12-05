#include "event_queue.h"

eStatus eventq_init(sEventQueue *queue) {    
    if (queue == NULL) { return kerror; }    
    _PROTECT(kinterrupt_save_flags);   
    queue->tail = 0;
    queue->head = 0;    
    _RELEASE(kinterrupt_restore_flags);
    return ksuccess;
}

void eventq_deinit(sEventQueue *queue) {
    (void) queue;
    return;
}

eStatus eventq_enqueue (sEventQueue* restrict queue, const sEvent* restrict event) {
    if ((queue == NULL) || (event == NULL)) { return kerror; }
    eStatus status = kfailure;
    _PROTECT(kinterrupt_save_flags);       
    uint8_t next_tail = (queue->tail + 1) & EVENT_QUEUE_LENGTH;
    if (next_tail != queue->head) {
        sEvent *e = &(queue->buffer[queue->tail]);                                 
        *e = *event;
        queue->tail = next_tail;        
        status = ksuccess;   
    }    
    _RELEASE(kinterrupt_restore_flags);
    return status;
}

eStatus eventq_dequeue(sEventQueue* restrict queue, sEvent* restrict event) {
    if ((queue == NULL) || (event == NULL)) { return kerror; }
    eStatus status = kfailure;
    _PROTECT(kinterrupt_save_flags);   
    if (queue->tail != queue->head) {
        *event = queue->buffer[queue->head];
        queue->head = (queue->head + 1) & EVENT_QUEUE_LENGTH;
        status = ksuccess;
    }    
    _RELEASE(kinterrupt_restore_flags);    
    return status;
}
