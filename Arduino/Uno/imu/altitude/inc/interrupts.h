/**
  * @file interrupts.h
  * @author Sushant Sundaresh
  * @date 2016-11-30  
  * @details
  * Interrupt protection and restoration wrappers.
  */

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

/**
  * @details 
  * Disables interrupts and does not save state to be restored if !will_restore.
  *
  * @param [in] will_restore Whether or not we plan to restore the old interrupt
  *              state afterwards.
  */
void _PROTECT(bool will_restore); 
/**
  * @details 
  * If restore is true, restores saved interrupt state. Otherwise, simply
  *  enables interrupts.
  *
  * @param [in] restore Whether or not we want to restore the old interrupt
  *              state, saved during a _PROTECT call.
  */
void _RELEASE(bool restore);

#endif
// _INTERRUPTS_H_