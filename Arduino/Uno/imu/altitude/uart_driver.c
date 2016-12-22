#include "uart_driver.h"

/* Local Functions */

/**
  * @details
  * UART packets are usually read via the bidirectional register 
  * URD0. This function, when compiled with AVR = 1, does exactly that.
  * When compiled for GCC (in silico testing), we allow this function to
  * be wrapped so that the tester can feed in whatever UART data is required,
  * then call something like ISR_USART_RX_MOCK. 
  * @param [in] uart an initialized uart object
  * @param [in] byte where to store the received byte
  * @return #kerror: an error occurred during byte rx
  * @return #ksuccess: a byte was successfully rx'd
  */
static eStatus uart_byte_buffer_receive_ (sUARTDriver *uart, uint8_t *byte) {
    (void) uart;    
    uint8_t rx_errors = 0;
#if AVR
    rx_errors = (UCSR0A & (RX_FRAME_ERROR | DATA_OVERRUN | PARITY_ERROR));
    *byte = UDR0;
#endif
    return (rx_errors > 0) ? kerror : ksuccess;
}

/** 
  * @details
  * UART packets are usually transmitted by writing the bidirectional register
  * URD0. This function, when compiled with AVR = 1, does exactly that.
  * when compiled for GCC (in silico testing), we allow this function to be
  * wrapped so that the tester can buffer the UART data being transmitted,
  * to compare against what they expected.
  * @param [in] uart an initialized uart object
  * @param [in] byte the data to transmit  
  */
static void uart_byte_buffer_transmit_ (sUARTDriver *uart, uint8_t byte) {
    (void) uart;
#if AVR
    UDR0 = byte;
#endif    
}

/** 
  * @details 
  * Checks for transmission errors then enqueues the received byte into the 
  * registered event queue if there are no errors. 
  * Called from an interrupt context. 
  *
  * @param [in] uart an initialized uart object
  */
static void uart_isr_receive_(sUARTDriver *uart) {
    uint8_t rx_byte;
    if (uart_byte_buffer_receive_(uart, &rx_byte) != ksuccess) { return; }
    sEvent rx_event = {
        .type = kevent_serial_rx;
        .data = rx_byte;
    }
    eventq_enqueue(uart->rx_client, &rx_event);    
}

/** 
  * @details 
  * Expects sUARTDriver to have been loaded with an sData structure to transmit,
  * and transmits this a byte at a time, incrementing tx_byte_num as it goes.
  * Sets a volatile uint8_t flag's LSB when it's done, an atomic operation.
  * @param [in] uart an initialized uart object  
  */
static void uart_isr_transmit_(sUARTDriver *uart) {
    if (uart->tx_byte_num >= sizeof(sData)) {
        uart->is_tx_done = (uint8_t) ktrue;
        return;
    }
    /* This is inefficient on purpose, to help me test by wrapping 
        the next function call to recursively call the ISR */
    uint8_t byte_index = uart->tx_byte_num;
    uart->tx_byte_num += 1; 
    uart_byte_buffer_transmit_(uart, *(((void *) uart->tx_data) + byte_index));       
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

void ISR_USART_RX_MOCK(sUARTDriver *uart) {
    uart_isr_receive(uart);
}

void ISR_USART_TX_MOCK(sUARTDriver *uart) {
    uart_isr_transmit(uart);
}

#endif

/* Public Functions */

eStatus uart_init(sUARTDriver* restrict uart, sEventQueue* restrict to_notify_on_rx) {
    if ((uart == NULL) || (to_notify_on_rx == NULL)) { return kerror; }
    _PROTECT(kinterrupt_save_flags);    
    uart->rx_client = to_notify_on_rx;
    uart->tx_byte_num = 0;
    uart->is_tx_done = (uint8_t) ktrue;
#if AVR
    UCSR0A = TX_COMPLETE;
    UCSR0B = TX_ENABLE | RX_ENABLE | RX_COMPLETE_IE | TX_COMPLETE_IE;
    UCSR0C = USART_PARITY | TXRX_CHAR_SIZE10 | TX_STOP_BITS;
    BRR0L  = BAUD_BASE_L;
    UBRR0H = BAUD_BASE_H;
#endif
    _RELEASE(kinterrupt_restore_flags);
    return ksuccess; 
} 

void uart_deinit(sUARTDriver *uart) {
    if (uart == NULL) { return; }
    _PROTECT(kinterrupt_save_flags);   
    uart->rx_client = NULL;
    uart->tx_byte_num = 0;
    uart->is_tx_done = (uint8_t) ktrue;
#if AVR
    UCSR0A = TX_COMPLETE;
    UCSR0B = 0;
#endif
    _RELEASE(kinterrupt_restore_flags);
}

void uart_log(sUARTDriver* restrict uart, const sData* restrict data) { 
    if ((eStatus) uart->is_tx_done != ktrue) { return kfailure; }
    uart->tx_byte_num = 0;
    uart->is_tx_done = (uint8_t) kfalse;
    uint8_t* internal_data = (uint8_t *) &(uart->tx_data);
    uint8_t* external_data = (uint8_t *) data;
    uint8_t k = 0;
    for (k = 0; k < sizeof(sData); k++) {
        *(internal_data + k) = *(external_data + k);
    }
    uart_isr_transmit_(uart);
    while (!(uart->is_tx_done == (uint8_t) ktrue)) { _SLEEP(); }
    return ksuccess;
}
