/**
  * @file test_timer_driver.c
  * @author Sushant Sundaresh
  * @date 2016-11-30
  * @details
  * Tests the timer driver in isolation prior to on-device testing.
  */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "timer_driver.h"

/* Unit under test */
#define MAX_CLIENTS 3
static sTimerDriver timer;
static sTimerClient clients[MAX_CLIENTS]; 
static sEventQueue client[MAX_CLIENTS];
static sTicks observed_ticks, test_ticks;

/* Helper Functions */
static int test_vector = 0;
static int check_number = 0;
static void _next_test_vector (void) {
    test_vector++;
    check_number = 0;
}

static bool _check(bool condition) {
    if (condition) {
        fprintf(stdout, "Vector %d Check %d Passed\n", test_vector, check_number);
        return true;
    } else {
        fprintf(stdout, "Vector %d Check %d Failed\n", test_vector, check_number);        
        return false;
    }
}

static void _drive_n_timer_interrupts(int n, sTicks *ticks) {
    int k, j;
    for (k = 0; k < n; k++) {
        ISR_TIMER(&timer);
        for (j = CLOCK_BYTE_WIDTH; j > -1; j++) {
            if (++ticks->count[j] != 0) { break; }
        }
    }
}

static void _clear_ticks (sTicks *ticks) {
    int k; for (k = CLOCK_BYTE_WIDTH; k > -1; k--) { ticks->count[k] = 0; }
}

static bool _compare_ticks (sTicks *a, sTicks *b) {
    bool res = true;
    int k;
    for (k = CLOCK_BYTE_WIDTH; k > -1; k--) { 
        res = res && (a->count[k] == b->count[k]); 
    }
    return res;
}

int main (int argc, char **argv) {    
    (void) argc; (void) argv;

    // Vector 1: Does the timer reject clients when its registry is full?
    if (!_check(ksuccess == timer_init(&timer, &clients[0], MAX_CLIENTS))) { return 1; } 
    int k;
    for (k = 0; k < MAX_CLIENTS; k++) {
        if (!_check(ksuccess == timer_register(&timer, &client[0], 1))) { return 1; } 
    }
    if (!_check(kfailure == timer_register(&timer, &client[0], 1))) { return 1; }
    timer_deinit(&timer);
    _next_test_vector();
    
    // Vector 2: Does the timer return and update its ticks?
    timer_init(&timer, &clients[0], MAX_CLIENTS);
    _clear_ticks(&test_ticks); // This is our test clock against which we compare the timer clock for accuracy
    if (!_check(ksuccess == timer_get_ticks (&timer, &observed_ticks))) { return 1; };     
    if (!_check(_compare_ticks(&test_ticks,&observed_ticks))) { return 1; }         
    _drive_n_timer_interrupts(2, &test_ticks);
    if (!_check(_compare_ticks(&test_ticks,&observed_ticks))) { return 1; }     
    _next_test_vector();

    return 0;
}