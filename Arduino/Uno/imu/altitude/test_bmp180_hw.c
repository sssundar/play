/**
  * @file test_bmp180_hw.c
  * @author Sushant Sundaresh
  * @date 2017-01-10
  * @details 
  * Tests getting calibration and temperature/pressure data from the BMP180.
  * 0. Set up the Adafruit 1604 board as shown in https://www.adafruit.com/product/1604
  * 1. Init TWI, Timer0, UART, the BMP180 driver, and an Event Queue
  * 2. Wait 1 second for BMP180 to initialize (requires 10 ms) then send START_TEST from laptop
  */    

#include "twi_driver.h"
#include "uart_driver.h"
#include "timer_driver.h"
#include "bmp180_driver.h"
#include "event_queue.h"
#include "test_helpers_hw.h"

#define HZ_1 200
#define HZ_10 20
#define HZ_200 1
#define START_TEST 's'

sTWIDriver twi;
sUARTDriver uart0;
sTimerDriver timer0;
sTimerClient clients[3];
sBMP180Driver barometer;
sEventQueue inbox;

void die_ (uint8_t caller_id, sData *log_data) {
    data_add_byte(log_data, caller_id);
    uart_log(&uart0, log_data);
    while (1) { _SLEEP(); } 
}

void main(void) __attribute__((noreturn));
void main(void) {         
    sEvent event;         
    sData assert_log = {
        .timestamp = { .count = {0x00, 0x00, 0x00} },
        .bytes = { 0x00, 0x00, 0x00 },
        .num_bytes = 0x00,
        .max_bytes = 1,
        .type = kdatatype_unset
    };    
    sData measurement = {
        .timestamp = { .count = {0x00, 0x00, 0x00} },
        .bytes = { 0x00, 0x00, 0x00 },
        .num_bytes = 0x00,
        .max_bytes = 0,
        .type = kdatatype_unset
    };    

    _ENABLE_SLEEP();    
    twi_init(&twi);        
    eventq_init(&inbox);    
    uart_init(&uart0, &inbox);       
    timer_init(&timer0, (sTimerClient *) clients, 3);    
    bmp180_init(&barometer); 
    
    eStatus have_sampled_temperature_this_second = kfalse;
    eStatus am_currently_sampling_temperature = kfalse;
    eStatus am_currently_sampling_pressure = kfalse;

    while (1) {
        while (ksuccess != eventq_dequeue(&inbox, &event)) {
            _SLEEP();
        } 

        if (event.type == kevent_serial_rx) {            
            if (event.data == START_TEST) {
                bmp180_get_calibration(&barometer, &twi, &uart0, &timer0);
                timer_register(&timer0, &inbox, HZ_200);  
                timer_register(&timer0, &inbox, HZ_10);      
                timer_register(&timer0, &inbox, HZ_1);  
            }            
        }

        if ((event.type == kevent_timer) && (event.data == HZ_1)) {
            have_sampled_temperature_this_second = kfalse;
        }

        if ((event.type == kevent_timer) && (event.data == HZ_10)) {                                                               
            if ((am_currently_sampling_temperature == kfalse) && (am_currently_sampling_pressure == kfalse)) {
                if (have_sampled_temperature_this_second == kfalse) {
                    bmp180_start_temperature_sampling(&barometer, &twi);         
                    am_currently_sampling_temperature = ktrue;
                } else {
                    bmp180_start_pressure_sampling(&barometer, &twi);
                    am_currently_sampling_pressure = ktrue;
                }                                
            }
        }

        if ((event.type == kevent_timer) && (event.data == HZ_200)) {             
            if (am_currently_sampling_temperature == ktrue) {                
                if (ktrue == bmp180_is_temperature_ready(&barometer, &twi)) {                                                    
                    bmp180_get_temperature_data(&barometer, &timer0, &twi, &measurement);                                        
                    uart_log(&uart0, &measurement);
                    have_sampled_temperature_this_second = ktrue;                    
                    am_currently_sampling_temperature = kfalse;
                }                                    
            }

            if (am_currently_sampling_pressure == ktrue) {                
                if (ktrue == bmp180_is_pressure_ready(&barometer, &twi)) {                                                    
                    bmp180_get_pressure_data(&barometer, &timer0, &twi, &measurement);                                        
                    uart_log(&uart0, &measurement);
                    am_currently_sampling_pressure = kfalse;
                }                                    
            }
        }
    }
    
    die_(0, &assert_log);
}


