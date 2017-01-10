/**
  * @file test_twi_hw.c
  * @author Sushant Sundaresh
  * @date 2017-01-09
  * @details 
  * Tests receiving and sending TWI transmissions to the BMP180.
  * 0. Set up the Adafruit 1604 board as shown in https://www.adafruit.com/product/1604
  * 1. Init TWI, UART, Event Queue
  * 2. Wait 1 second for BMP180 to initialize (requires 10 ms) then send START_TEST from laptop
  * 3. Run the following TWI commands
  *     START 
  *     SLA_W 
  *     (TX DATA=0xAA) 
  *     RESTART 
  *     SLA_R 
  *     (RX DATAx2 = Parameter AC1, MSB (0xAA) then LSB (0xAB))
  *     NACK 
  *     STOP
  *  If all TWI interrupts pass with valid statuses, and we retrieve data 
  *  that is different between the MSB and LSB and is not 0x00 of 0xFF,
  *  the test is a success and a UART transmission to that effect will be sent.
  * @aside
  * Passed on 2017-01-10 after switching from interrupt-sleeping to busy-waiting,
  * and slowing the TWI clock to 100kHz from 1MHz. The first of these changes
  * was not necessary; it was just expedient given the lack of an ISR friendly TWI FSM 
  * in my current "drive it yourself" design.
  */    

#include <avr/io.h>
#include "twi_driver.h"
#include "uart_driver.h"
#include "event_queue.h"
#include "data.h"
#include "test_helpers_hw.h"

#define HZ_1 200
#define BMP180_SLA 0xEE // = 0x77 (7-bit notation) << 1 = 0b01110111 << 1 = 0b11101110 = 0xEE
#define BMP180_AC1 0xAA 
#define START_TEST 's'

sTWIDriver twi;
sUARTDriver uart0;
sEventQueue inbox;

void die_ (uint8_t caller_id, sData *data) {
    data->timestamp.count[0] = caller_id; 
    data->timestamp.count[1] = TWSR;
    uart_log(&uart0, data);
    while (1) { _SLEEP(); } 
}

void main(void) __attribute__((noreturn));
void main(void) {     
    _ENABLE_SLEEP();

    twi_init(&twi);
    eventq_init(&inbox);    
    uart_init(&uart0, &inbox);       
    sEvent event;         
    sData data = {
        .timestamp = { .count = {0x00, 0x00, 0x00} },
        .bytes = { 0x00, 0x00, 0x00 },
        .num_bytes = 0x00,
        .max_bytes = BYTEWIDTH_BMP180_CALIBRATION,
        .type = kdatatype_bmp180_calibration
    };    

    while (1) {
        while (ksuccess != eventq_dequeue(&inbox, &event)) {
            _SLEEP();
        } 

        if (event.type == kevent_serial_rx) {            
            if (event.data == START_TEST) {
                break;
            }            
        }
    }
    
    uint8_t tx_data = BMP180_AC1;
    uint8_t rx_data[2] = {0};

    if (ksuccess != twi_tx_start(&twi, 0))                      { die_(1,&data); }
    if (ksuccess != twi_tx_sla(&twi, BMP180_SLA, 0))            { die_(2,&data); }    
    if (ksuccess != twi_tx_data(&twi, &tx_data, 1, kfalse))     { die_(3,&data); }    
    if (ksuccess != twi_tx_start(&twi, 1))                      { die_(4,&data); }    
    if (ksuccess != twi_tx_sla(&twi, BMP180_SLA, 1))            { die_(5,&data); }
    if (ksuccess != twi_rx_data(&twi, (uint8_t *) &rx_data, 2)) { die_(6,&data); }
    
    if ((rx_data[0] == 0x00) || (rx_data[0] == 0xFF))           { die_(7,&data); }
    if ((rx_data[1] == 0x00) || (rx_data[1] == 0xFF))           { die_(8,&data); }

    data_add_byte(&data, rx_data[0]);
    data_add_byte(&data, rx_data[1]);
    data_add_byte(&data, BMP180_AC1);    
    uart_log(&uart0, &data);
    while (1) { _SLEEP(); } 
}


