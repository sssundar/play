/**
  * @file twi_driver.h
  * @author Sushant Sundaresh
  * @date 2016-12-22
  * @details 
  * The driver for the TWI module of the ATMEGA328p processor.
  * Relies on register LD/ST addresses defined in io328p.h (imported in io.h)
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

/* TWSR - TWI Status Register */
#define TWSR_PRESCALER  0x03 // Prescaler bits, set to 0 for prescale factor of 1
#define TWSR_STATUS     0xF8 // Status bits, read only (see pages 229, 233 of Atmega 328p reference manual)
#define MASTER_START_TRANSMITTED         0x08
#define MASTER_ARBITRATION_LOST          0x38
#define MASTER_TX_SLAW_ACKD              0x18
#define MASTER_TX_SLAW_NACKD             0x20
#define MASTER_TX_DATA_ACKD              0x28
#define MASTER_TX_DATA_NACKD             0x30
#define MASTER_RX_SLAR_ACKD              0x40
#define MASTER_RX_SLAR_NACKD             0x48
#define MASTER_RX_DATA_ACKD              0x50
#define MASTER_RX_DATA_NACKD             0x58

/* End Two Wire Interface Register Bit Masks */

// @details
// TWI driver object that really just stores one-hot FSM state variables.
#define TWI_IS_DISABLED         0x01    
#define TWI_IS_IDLE             0x02
#define TWI_IS_SENDING_START    0x04    
#define TWI_IS_SENDING_SLA_R    0x08
#define TWI_IS_SENDING_SLA_W    0x10
#define TWI_IS_SENDING_DATA     0x20    
#define TWI_IS_RECEIVING_DATA   0x40 
#define TWI_FAILED              0x80
typedef struct sTWIDriver {    
    uint8_t state;
} sTWIDriver;

// @details
// Protects itself from interrupts but restores interrupt flags on return.
// Sets up TWI peripheral in master transmitter/receiver mode. 
// Resets state variables related to the TWI tx/rx FSM. 
// @param [in] twi a TWI driver object, uninitialized
// @return #ksuccess: successfully initialized TWI peripheral
// @return #kerror: if twi is null
// @todo
//  Set TWBR to 0 to have a 1 MHz TWI base clock
eStatus twi_init(sTWIDriver *twi);                                         

// Just disable TWI enable bit in the TWCR and clear the TWINT bit 
void twi_deinit(sTWIDriver *twi);                                          

// Not critical. Will not return until the TWI command is complete.
//  Set TWCR to 
eStatus twi_get(sTWIDriver* restrict twi, uint8_t addr, uint8_t reg, uint8_t* restrict dest); 

// Not critical. Will not return until the TWI command is complete.
eStatus twi_put(sTWIDriver *twi, uint8_t addr, uint8_t reg, uint8_t src);  

#endif
// _TWI_DRIVER_H_
