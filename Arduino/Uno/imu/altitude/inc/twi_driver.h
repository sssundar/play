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

// @todo Document, Implement
typedef struct sTWIDriver {
	char state[4];
} sTWIDriver;

// @details
// Protects itself from interrupts but restores interrupt flags on return.
// Resets TWI peripheral and state variables related to the TWI tx/rx FSM. 
// @param [in] twi a TWI driver object, uninitialized
// @return #ksuccess: successfully initialized TWI peripheral
// @return #kerror: if twi is null
eStatus twi_init(sTWIDriver *twi);                                         
// @todo Document, Implement
// PROTECTS ITSELF and restores interrupt flags. 
void twi_deinit(sTWIDriver *twi);                                          
// @todo Document, Implement
// Not critical. Will not return until the TWI command is complete.
eStatus twi_get(sTWIDriver* restrict twi, uint8_t addr, uint8_t reg, uint8_t* restrict dest); 
// @todo Document, Implement
// Not critical. Will not return until the TWI command is complete.
eStatus twi_put(sTWIDriver *twi, uint8_t addr, uint8_t reg, uint8_t src);  

#endif
// _TWI_DRIVER_H_
