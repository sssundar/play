#include "uart_driver.h"

/* Local Functions */

/** 
  * @details 
  * Checks for transmission errors such as
  *  (UCSR0A & (RX_FRAME_ERROR | DATA_OVERRUN | PARITY_ERROR)) > 0 
  * then enqueues the received byte into the registered event queue if there
  * are no errors. Called from an interrupt context. 
  * @param [in] uart an initialized uart object
  */
static void uart_isr_receive(sUARTDriver *uart) {

}

/** 
  * @details 
  * Expects sUARTDriver to have been loaded with an sData structure to transmit,
  * and transmits this a byte at a time, incrementing tx_byte_num as it goes.
  * Sets a volatile uint8_t flag's LSB when it's done, an atomic operation.
  * @param [in] uart an initialized uart object
  */
static void uart_isr_transmit(sUARTDriver *uart) {

}

#if AVR
/**
  * @details
  * Byte receive complete interrupt routine for UART0, controlled by this driver
  */
ISR(USART_RX_vect) {                        
    uart_isr_receive(&uart0);
}

/**
  * @details
  * Byte transmit complete interrupt routine for UART0, controlled by this driver
  */
ISR(USART_TX_vect) {        
    uart_isr_transmit(&uart0);
}

#else

void ISR_UART_RX(sUARTDriver *uart) {
    uart_isr_receive(uart);
}

void ISR_UART_TX(sUARTDriver *uart) {
    uart_isr_transmit(uart);
}
#endif

/* Public Functions */

eStatus uart_init(sUARTDriver* restrict uart, sEventQueue* restrict to_notify_on_rx) {
} 

void uart_deinit(sUARTDriver *uart) {

}

void uart_log(sUARTDriver* restrict uart, const sData* restrict data) { 

}
