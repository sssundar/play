/**
  * @file interrupts.h
  * @author Sushant Sundaresh
  * @date 2016-11-30  
  * @details
  * Interrupt protection and restoration wrappers.
  * Sleep wrappers for interrupt driven event handling.
  */

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "config.h"
#include "types.h"

#if AVR
#include <avr/io.h> 
#include <avr/interrupt.h>
#endif

typedef enum eInterruptRequest {
  kinterrupt_do_not_save_flags,
  kinterrupt_save_flags,
  kinterrupt_do_not_restore_flags,
  kinterrupt_restore_flags  
} eInterruptRequest;

/**
  * @details 
  * Disables interrupts and does not save state to be restored if !will_restore.
  *
  * @param [in] save_flags Whether or not we plan to restore the old interrupt
  *              state afterwards.
  */
void _PROTECT(eInterruptRequest save_flags); 
/**
  * @details 
  * If restore is true, restores saved interrupt state. Otherwise, simply
  *  enables interrupts.
  *
  * @param [in] restore_flags Whether or not we want to restore the old interrupt
  *              state, saved during a _PROTECT call.
  */
void _RELEASE(eInterruptRequest restore_flags);

/**
  * @details 
  * Allow low-power mode to idle-loop instead of busy-loop
  */
void _ENABLE_SLEEP(void);

/**
  * @details
  * Puts the MCU into an idle sleep state, from which it can be woken by 
  * interrupts.
  */
void _SLEEP(void);

#endif
// _INTERRUPTS_H_