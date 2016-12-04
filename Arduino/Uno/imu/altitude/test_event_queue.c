/**
  * @file test_event_queue.c
  * @author Sushant Sundaresh
  * @date 2016-12-03
  * @details
  * Tests the event queue in isolation prior to on-device testing.
  */

#include "test_helpers.h"
#include "event_queue.h"

/* Unit under test */
static sEventQueue uut;
static sEvent observed_event, test_event;

/* Output Parsing */
static void create_event_(sEvent *e, eEventType type, uint8_t data) {
    e->type = type;
    e->data = data;
}

static void clear_event_(sEvent *e) {
    e->type = 0;
    e->data = 0;
}

static bool compare_events_(sEvent *a, sEvent *b) {
    return (a->type == b->type) && (a->data == b->data);
}

/* Unit Test */
int main (int argc, char **argv) {    
    (void) argc; (void) argv;

    /* Vector 0: Initialization */
    if (!check(kerror == eventq_init(NULL))) { return 1; }
    if (!check(ksuccess == eventq_init(&uut))) { return 1; }
    increment_test_vector();

    eStatus eventq_enqueue(sEventQueue* restrict queue, const sEvent* restrict event);
    eStatus eventq_dequeue(sEventQueue* restrict queue, sEvent* restrict event);   

    /* Vector 1: Empty queue enq and deq */
    create_event_(&test_event, kevent_serial_rx, 1);
    clear_event_(&observed_event);
    if (!check(kfailure == eventq_dequeue(&uut, &observed_event))) { return 1; }   
    if (!check(ksuccess == eventq_enqueue(&uut, &test_event))) { return 1; }       
    if (!check(ksuccess == eventq_dequeue(&uut, &observed_event))) { return 1; }   
    if (!check(compare_events_(&test_event, &observed_event))) { return 1; }
    increment_test_vector();   

    /* Vector 2: Test full and empty conditions */
    eventq_init(&uut);
    int k = 0;
    do {
        create_event_(&test_event, k % 2 == 1 ? kevent_serial_rx : kevent_timer, k); 
        k++;
        fprintf(stderr, "k: %d, tail: %d, head: %d\n", k, uut.tail, uut.head);
    } while (eventq_enqueue(&uut, &test_event) == ksuccess);
    
    if (!check(EVENT_QUEUE_LENGTH == k)) { return 1; }

    k--;
    while (eventq_dequeue(&uut, &observed_event) == ksuccess) {
        create_event_(&test_event, k % 2 == 1 ? kevent_serial_rx : kevent_timer, k); 
        if (!check(compare_events_(&test_event, &observed_event))) { return 1; }
        k--;   
    }                

    if (!check(0 == k)) { return 1; }

    /* Vector 3: Test wraparound */
    eventq_init(&uut);
    k = 0;
    do {
        create_event_(&test_event, kevent_timer, k); 
        k++;
    } while (eventq_enqueue(&uut, &test_event) == ksuccess);
    eventq_dequeue(&uut, &observed_event);
    eventq_dequeue(&uut, &observed_event);
    do {
        create_event_(&test_event, kevent_timer, k); 
        k++;
    } while (eventq_enqueue(&uut, &test_event) == ksuccess);

    if (!check(uut.buffer[0].data == k-1)) { return 1; }    

    return 0;
}