/**
  * @file twi_driver.h
  * @author Sushant Sundaresh
  * @date 2016-12-22
  * @details 
  * The hardware abstraction layer for the TWI module of the ATMEGA328p processor.
  * Relies on register LD/ST addresses defined in io328p.h (imported in io.h)
  * You MUST enable sleep (_ENABLE_SLEEP()) if you want to use this driver.
  */

#ifndef _TWI_DRIVER_H_
#define _TWI_DRIVER_H_

#include "config.h"
#include "stdmath.h"
#include "types.h"
#include "interrupts.h"
#include "io.h"

/* Begin Two Wire Interface Register Bit Masks */

/* TWBR, TWDR are defined in io.h */

/* TWCR - TWI Control Register */
#define TWCR_TWINT  0x40    // Write 1 to release SCL line after processing an event
#define TWCR_TWEA   0x30    // Sends Acknowledge Bit, can just set this to 1 in master tx/rx mode
#define TWCR_TWSTA  0x20    // Sends Start Bit, must be cleared
#define TWCR_TWSTO  0x10    // Sends a Stop bit, auto-cleared
#define TWCR_TWWC   0x08    // Can probably ignore unless there are bus errors
#define TWCR_TWEN   0x04    // Enables TWI peripheral
#define TWCR_TWIE   0x01    // Interrupt enable
#define TWCR_BASE   (TWCR_TWINT | TWCR_TWEN | TWCR_TWIE)

/* TWSR - TWI Status Register */
#define TWSR_PRESCALER  0x03 // Prescaler bits, set to 0 for prescale factor of 1
#define TWSR_STATUS     0xF8 // Status bits, read only (see pages 229, 233 of Atmega 328p reference manual)
#define MASTER_START_TRANSMITTED            0x08
#define MASTER_REPEATED_START_TRANSMITTED   0x10
#define MASTER_ARBITRATION_LOST             0x38
#define MASTER_TX_SLAW_ACKD                 0x18
#define MASTER_TX_SLAW_NACKD                0x20
#define MASTER_TX_DATA_ACKD                 0x28
#define MASTER_TX_DATA_NACKD                0x30
#define MASTER_RX_SLAR_ACKD                 0x40
#define MASTER_RX_SLAR_NACKD                0x48
#define MASTER_RX_DATA_ACKD                 0x50
#define MASTER_RX_DATA_NACKD                0x58

/* End Two Wire Interface Register Bit Masks */

// @details
// TWI driver object that really just stores status codes from the last TWI action
typedef struct sTWIDriver {    
    uint8_t is_busy;
    uint8_t status;
} sTWIDriver;

#if AVR
// @details
// Statically allocated TWI peripheral object
extern sTWIDriver twi;
#endif 

// @details
// Protects itself from interrupts but restores interrupt flags on return.
// Sets up TWI peripheral in master transmitter/receiver mode. 
// Sets TWBR to 0 to have a 1 MHz TWI base clock
// Resets state variables related to the TWI tx/rx FSM. 
// @param [in] twi a TWI driver object, uninitialized
// @return #ksuccess: successfully initialized TWI peripheral
// @return #kerror: if twi is null
eStatus twi_init(sTWIDriver *twi);                                         

// @details
// Protects itself from interrupts but restores interrupt flags on return.
// Just clear the TWINT bit and disable TWI enable bit in the TWCR
// @param [in] twi a TWI driver object
void twi_deinit(sTWIDriver *twi);                                          

// @details
// Sets up the TWI start signal and waits for the interrupt indicating this 
// has been sent. Gets the status code.
// @param [in] twi a TWI driver object
// @param [in] is_repeated is this a repeated start
// @return #ksuccess on success
// @return #kerror otherwise
eStatus twi_tx_start (sTWIDriver *twi, uint8_t is_repeated);

// @details
// Sets up the TWI SLA(W/R) signal and waits for the interrupt indicating this 
// has been sent. Gets the status code.
// @param [in] twi a TWI driver object
// @param [in] addr TWI bus address
// @param [in] is_write is this a write (not a read)
// @return #ksuccess on success
// @return #kerror otherwise
eStatus twi_tx_sla (sTWIDriver *twi, uint8_t addr, uint8_t is_write);

// @details
// Sets up the TWDR and sends the data. Waits for the interrupt indicating this 
// has been sent. Gets the status code, and repeats till it's out of bytes to send.
// Then, transmits the stop condition and releases the bus.
// @param [in] twi a TWI driver object
// @param [in] data bytes to transmit
// @param [in] num_bytes number of bytes to transmit
// @param [in] stop_after_tx if ktrue, then a stop condition will be transmitted after the tx is complete
// @return #ksuccess on success
// @return #kerror otherwise
eStatus twi_tx_data (sTWIDriver* restrict twi, const uint8_t* restrict data, uint8_t num_bytes, eStatus stop_after_tx);

// @details
// Receives num_bytes bytes then releases the bus.
// @param [in] twi a TWI driver object
// @param [in] data bytes to receive
// @param [in] num_bytes number of bytes to receive
// @return #ksuccess on success
// @return #kerror otherwise
eStatus twi_rx_data (sTWIDriver* restrict twi, uint8_t* restrict data, uint8_t num_bytes);

#endif
// _TWI_DRIVER_H_
