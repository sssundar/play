/**
  * @file interrupts.h
  * @details
  * Chooses between hardware and mocked interrupt handling
  * and protection.
  */

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "config.h"

#if AVR
#include <avr/io.h> 
#include <avr/interrupt.h>
#else
#include "stubbed_avr_io.h"
#include "stubbed_avr_interrupt.h"
#endif

#define PROTECT(...) (uint8_t flags = SREG; cli();)

#endif