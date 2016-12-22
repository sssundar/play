/**
  * @file test_uart_driver.c
  * @author Sushant Sundaresh
  * @date 2016-12-21
  * @details
  * Tests the UART driver in isolation prior to on-device testing.
  */

#include "test_helpers.h"  
#include "uart_driver.h"

/* Unit under test */
static sUARTDriver uut;

/* Simulation Function Wrappers, to mimic UART interrupts when 
   these are called:
        void ISR_USART_RX_MOCK(sUARTDriver *uart);
        void ISR_USART_TX_MOCK(sUARTDriver *uart); */
static uint8_t rx_byte_intended;
eStatus __wrap_uart_byte_buffer_receive_ (sUARTDriver *uart, uint8_t *byte) {
    (void) uart;
    *byte = rx_byte_intended;
    return 0;
}

static uint8_t rx_byte_observed;
eStatus __wrap_eventq_enqueue (sEventQueue *queue, const sEvent *event) {
    (void) queue;
    rx_byte_observed = event->data;
    return ksuccess;
}

static uint8_t tx_bytes[sizeof(sData)];
static uint8_t len_tx_bytes;
void __wrap_uart_byte_buffer_transmit_ (sUARTDriver *uart, uint8_t byte) {
    if (len_tx_bytes >= MAX_BUFFERED_BYTES) { return; }
    tx_bytes[len_tx_bytes] = byte;
    len_tx_bytes += 1;
    ISR_USART_TX_MOCK(uart);
}

/* Unit test */
int main (int argc, char **argv) {    
    (void) argc; (void) argv;
    sEventQueue ignored_client;
    sData test_data = {
        .timestamp = {0};
        .bytes = {1};
        .num_bytes = 3;
        .max_bytes = 3;
        .type = kdatatype_bmp180_barometry;
    }

    // Vector 0: Does the UART driver transmit the correct bytes?
    if (!check(ksuccess == uart_init(&uut, &ignored_client))) { return 1; } 
    if (!check(ksuccess == uart_log(&uut, &test_data))) { return 1; }
    bool was_transmission_correct;
    int k;
    for (k = 0; k < sizeof(sData); k++) {
        was_transmission_correct = (tx_bytes[k] == *(((void *) test_data) + k));        
        if (!check(was_transmission_correct) { return 1; }
    }
    uart_deinit(&uut);
    increment_test_vector();

    // Vector 1: Does the UART driver receive the correct byte?
    uart_init(&uut, &ignored_client);
    rx_byte_intended = 'a';
    rx_byte_observed = 0;
    ISR_USART_RX_MOCK(&uut);
    if (!check(rx_byte_intended == rx_byte_observed)) { return 1; }
    uart_deinit(&uut);
    increment_test_vector();

    return 0;
}