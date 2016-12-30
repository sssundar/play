/**
  * @file test_uart_hw.c
  * @author Sushant Sundaresh
  * @date 2016-12-29
  * @details 
  *  Tests receiving and sending UART transmissions.
  */    
  
#include "uart_driver.h"
#include "timer_driver.h"
#include "event_queue.h"
#include "data.h"

#define START_SAMPLING 's'
#define END_SAMPLING 'e'
sUARTDriver uart0;
#define HZ_20 10    // 10 tick period for 20 Hz events off a 200 Hz timer
sTimerDriver timer0;
sTimerClient clients[1];
sEventQueue inbox;

void main(void) __attribute__((noreturn));
void main(void) {     
    _ENABLE_SLEEP();

    /*  Set up Digital Pin 6 (Uno R3), i.e. PD6, as an output, no pull-up,
        so it can be used for LED-debugging */
    DDRD |= 0x40;                       // PD6 is an output 
    PORTD &= 0xBF;                      // Clear PD6's output
    // To make PD6 toggle for debugging: use PIND |= 0x40;  
    // and place a 1kOhm + LED off Digital Pin 6 with a loop to GND

    eventq_init(&inbox);    
    timer_init(&timer0, (sTimerClient *) clients, 1);        
    uart_init(&uart0, &inbox);      
    timer_register(&timer0, &inbox, HZ_20);  
    sEvent event;         
    sTicks timestamp;
    sData data = {
        .timestamp = { .count = {0x01, 0x02, 0x03} },
        .bytes = { 0x04, 0x05, 0x06 },
        .num_bytes = 0x07,
        .max_bytes = 0x08,
        .type = 0x09
    };
    
    // Comment these out if you aren't even sure UART transmissions are working
    // or are unclear on the byte packing for the sData struct.
    data_set_type(&data, kdatatype_bmp180_barometry);
    data_add_byte(&data, 0x01);  
    
    uint8_t should_transmit = 0;

    while (1) {
        while (ksuccess != eventq_dequeue(&inbox, &event)) {
            _SLEEP();
        } 

        if ((event.type == kevent_timer) && (event.data == HZ_20)) {            
            if (should_transmit) {
                timer_get_ticks(&timer0, &timestamp);
                data_set_ticks(&data, &timestamp);  
                uart_log(&uart0, &data);    
            }            
        }                

        if (event.type == kevent_serial_rx) {            
            if (event.data == START_SAMPLING) {
                should_transmit = 1;        
            }            

            if (event.data == END_SAMPLING) {
                should_transmit = 0;        
            }            
        }
    }
}
