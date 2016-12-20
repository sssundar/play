/**
  * @file uart_driver.h
  * @author Sushant Sundaresh
  * @date 2016-12-19
  * @details 
  * The driver for the USART0 module of the ATMEGA328p processor.
  * Relies on register LD/ST addresses defined in io328p.h (imported in io.h)
  */

#ifndef _UART_DRIVER_H_
#define _UART_DRIVER_H_

#include "config.h"
#include "stdmath.h"
#include "types.h"
#include "interrupts.h"
#include "io.h"
#include "event_queue.h"
#include "data.h"

/****************************/
/* USART0 Control Registers */
/****************************/

/* UCSR0A                   Mask    Notes */
#define RX_COMPLETE         0x80    // R   r1 = unread data in RX buffer. 0 = empty.                        
#define TX_COMPLETE         0x40    // R/W r1 = empty tx buffer// w1 = clear it. can gen. tx interrupt
#define TX_BUFFER_EMPTY     0x20    // R   r1 = tx buffer can be written. can gen (UDRIE) interrupt
#define RX_FRAME_ERROR      0x10    // R   r1 = error. Valid until UDR0 read. w0 on UCSR0A write.
#define DATA_OVERRUN        0x08    // R   r1 = error. Valid until UDR0 read.
#define PARITY_ERROR        0x04    // R   r1 = error. Valid until UDR0 read.
#define DOUBLE_BAUD         0x02    // R/W w0 for synchronous. only in async CAN w1. ignore now.    
#define MULTI_PC_COMM       0x01    // R/W ignore now, write 0.
/* UCSR0B */
#define RX_COMPLETE_IE      0x80    // R/W w1 = interrupt on rx_complete
#define TX_COMPLETE_IE      0x40    // R/W w1 = interrupt on tx complete
#define TX_BUFF_READY_IE    0x20    // R/W w1 = interrupt on tx data register ready
#define RX_ENABLE           0x10    // R/W w0 = disable
#define TX_ENABLE           0x08    // R/W w1 = enable
#define TXRX_CHAR_SIZE2     0x04    // R/W bit 2 of 3, 1:0 in UCSR0C. make this 0 bit 8 bit
#define RX_BIT8             0x02    // R ignore
#define TX_BIT8             0x01    // R/W ignore
/* UCSR0C */
#define USART_MODE_SEL      0xC0    // R/W w0 for UART
#define USART_PARITY        0x30    // R/W w 11 = odd parity
#define TX_STOP_BITS        0x08    // R/W w0 = 1 stop bit
#define TXRX_CHAR_SIZE10    0x06    // R/W w 1 to both for 8 bit
#define USART_CLOCK_POL     0x01    // R/W w 0 for asynchronous mode
/* UBRRO{H,L} */
#define BAUD_BASE_H         0x00    // UBRR0H 
#define BAUD_BASE_L         0x67    // UBRR0L if you want 9600 baud at 16MHz

/**
  * @details 
  * A UART Driver object 
  */
typedef struct sUARTDriver {                
    sEventQueue *rx_client; // Client to be notified on UART byte 
                            //  reception events
    sData tx_data;          // Data to transmit
    uint8_t tx_byte_num;    // Which byte of the data
                            //  are we currently transmitting
} sUARTDriver;

#if AVR
/** 
  * @details 
  * Global statically allocated uart0 driver object that must be
  * allocated and initialized by the user of this driver.
  */
extern sUARTDriver uart0;
#endif

#if AVR == 0
/**
  * @details
  * Mocked interrupt service routine for UART0 receive complete events, 
  *  controlled by this driver
  * @param [in] uart an initialized uart driver object
  */
void ISR_UART_RX(sUARTDriver *uart);
/**
  * @details
  * Mocked interrupt service routine for UART0 transmit complete events, 
  *  controlled by this driver
  * @param [in] uart an initialized uart driver object
  */
void ISR_UART_TX(sUARTDriver *uart);
#endif 

/**
  * @details 
  * Protects itself from interrupts.
  *
  * Register    initializes to
  * UDR0        (nothing, this is where you transmit/receive data)
  * UCSR0A      TX_COMPLETE
  * UCSR0B      TX_ENABLE | RX_ENABLE | RX_COMPLETE_IE | TX_COMPLETE_IE
  * UCSR0C      USART_PARITY | TXRX_CHAR_SIZE10 | TX_STOP_BITS
  * UBRR0L      BAUD_BASE_L
  * UBRR0H      BAUD_BASE_H
  * for 9600 baud UART Tx/Rx, 8 bit, 1 stop bit, odd parity check. 
  *
  * @param [in] uart an uninitialized uart driver object
  * @param [in] client an event queue to notify when UART packets are received
  * @return #kerror: uart or to_notify_on_rx are null
  * @return #ksuccess: otherwise
  */
eStatus uart_init(sUARTDriver* restrict uart, sEventQueue* restrict to_notify_on_rx);  

/**
  * @details 
  * Protects itself from interrupts. Turns off UART0.
  *
  * @param [in] uart an initialized uart driver object
  */                                    
void uart_deinit(sUARTDriver *uart);                                    

/** 
  * @details 
  * Transmits the sample data in its entirety, byte by byte, then returns.
  * This call is blocking from the perspective of the main loop but does not
  * protect itself from interrupts. It is transmitting roughly 7 bytes of data 
  * at 9600 baud, which takes about 6 ms. 
  * @param [in] uart an initialized uart object
  * @param [in] data the data to transmit
  */
void uart_log(sUARTDriver* restrict uart, const sData* restrict data);    

#endif 
// _UART_DRIVER_H_
