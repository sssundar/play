/**
  * @file uart_hal.h
  * @author Sushant Sundaresh
  * @date 2016-12-22
  * @details 
  * Hardware abstraction layer for the tx/rx UART register to allow these
  * accesses to be wrapped for sw unit testing.
  * Relies on register LD/ST addresses defined in io328p.h (imported in io.h)
  */

#ifndef _UART_HAL_H_
#define _UART_HAL_H_

#include "config.h"
#include "stdmath.h"
#include "types.h"
#include "interrupts.h"
#include "io.h"
#include "uart_driver.h"

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
eStatus uart_byte_buffer_receive_ (sUARTDriver *uart, uint8_t *byte);

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
void uart_byte_buffer_transmit_ (sUARTDriver *uart, uint8_t byte);

#endif 
// _UART_HAL_H_