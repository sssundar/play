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
#define MAX_CLOCK_TICKS 256*256*255+256*255+255 // with a 24 bit clock
static sEvent observed_event, test_event;

/* Simulation Driver and Output Parsing */

/* Unit Test */
int main (int argc, char **argv) {    
    (void) argc; (void) argv;



    return 0;
}