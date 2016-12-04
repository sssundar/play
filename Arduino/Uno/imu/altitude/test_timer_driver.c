/**
  * @file test_timer_driver.c
  * @author Sushant Sundaresh
  * @date 2016-11-30
  * @details
  * Tests the timer driver in isolation prior to on-device testing.
  */

#include "test_helpers.h"  
#include "timer_driver.h"

/* Unit under test */
#define MAX_CLIENTS 3
static sTimerDriver timer;
static sTimerClient clients[MAX_CLIENTS]; 
static sEventQueue client[MAX_CLIENTS];
#define MAX_CLOCK_TICKS 256*256*255+256*255+255 // with a 24 bit clock
static sTicks observed_ticks, test_ticks;

/* Simulation driver and output parsing */
static void drive_n_timer_interrupts_(int n, sTicks *ticks) {
    int k;
    for (k = 0; k < n; k++) {
        ISR_TIMER(&timer);        
        int j;
        for (j = CLOCK_BYTE_WIDTH-1; j > -1; j--) {
            ticks->count[j]++;            
            if (!(ticks->count[j] == 0)) { 
                break; 
            }
        }
    }    
}

static void clear_ticks_ (sTicks *ticks) {
    int k; for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) { ticks->count[k] = 0; }
}

static bool compare_ticks_ (sTicks *a, sTicks *b) {
    bool res = true;
    int k;
    for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {     
        res = res && (a->count[k] == b->count[k]); 
    }
    return res;
}

/* Intended to test client notifications */
static int notification_count;
eStatus __wrap_eventq_enqueue (sEventQueue *queue, const sEvent *event) {
    (void) queue; (void) event;
    notification_count++;
    return ksuccess;
}

/* Unit test */
int main (int argc, char **argv) {    
    (void) argc; (void) argv;

    // Vector 0: Does the timer reject clients when its registry is full?
    if (!check(ksuccess == timer_init(&timer, &clients[0], MAX_CLIENTS))) { return 1; } 
    int k;
    for (k = 0; k < MAX_CLIENTS; k++) {
        if (!check(ksuccess == timer_register(&timer, &client[0], 1))) { return 1; } 
    }
    if (!check(kfailure == timer_register(&timer, &client[0], 1))) { return 1; }
    timer_deinit(&timer);
    increment_test_vector();
    
    // Vector 1: Does the timer return and update its ticks?
    timer_init(&timer, &clients[0], MAX_CLIENTS);
    clear_ticks_(&test_ticks); // This is our test clock against which we compare the timer clock for accuracy
    if (!check(ksuccess == timer_get_ticks (&timer, &observed_ticks))) { return 1; };     
    if (!check(compare_ticks_(&test_ticks,&observed_ticks))) { return 1; }         
    drive_n_timer_interrupts_(1, &test_ticks);
    timer_get_ticks (&timer, &observed_ticks);        
    if (!check(compare_ticks_(&test_ticks,&observed_ticks))) { return 1; }         
    drive_n_timer_interrupts_(MAX_CLOCK_TICKS-1, &test_ticks);
    timer_get_ticks (&timer, &observed_ticks);        
    if (!check(compare_ticks_(&test_ticks,&observed_ticks))) { return 1; }     
    timer_deinit(&timer);
    increment_test_vector();

    // Vector 2: Do we send notifications out to clients at the right times?
    notification_count = 0;
    timer_init(&timer, &clients[0], MAX_CLIENTS);
    clear_ticks_(&test_ticks);
    for (k = 0; k < MAX_CLIENTS; k++) { timer_register(&timer, &client[k], k+1); }
    drive_n_timer_interrupts_(2*MAX_CLIENTS,&test_ticks);    
    if (!check(notification_count == 11)) { return 1; }    // This is a magic number, calculated for 
                                                            // MAX_CLIENTS = 3 run registered for [1,2,3] tick 
                                                            // notifications, for 6 interrupts. 6+3+2 = 11.
    timer_deinit(&timer);
    increment_test_vector();

    return 0;
}