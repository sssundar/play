#include "uart_hal.h"

eStatus uart_byte_buffer_receive_ (sUARTDriver *uart, uint8_t *byte) {
    (void) uart;    
    uint8_t rx_errors = 0;
#if AVR
    rx_errors = (UCSR0A & (RX_FRAME_ERROR | DATA_OVERRUN | PARITY_ERROR));
    *byte = UDR0;
#else
    (void) byte;
#endif
    return (rx_errors > 0) ? kerror : ksuccess;
}

void uart_byte_buffer_transmit_ (sUARTDriver *uart, uint8_t byte) {
    (void) uart;
#if AVR
    UDR0 = byte;
#else
    (void) byte;
#endif
}