/**
  * @file io.h
  * @author Sushant Sundaresh
  * @date 2016-11-30  
  * @details
  * If the config.h header has AVR defined we
  * use the standard AVR definitions of MCU IO registers.
  * Otherwise we define dummy ones here so the code
  * in this project still compiles for off-platform 
  * testing under gcc. 
  */

#ifndef _IO_H_
#define _IO_H_

#include "config.h"

#if AVR
#include <avr/io.h> 
#endif

#endif
// _IO_H_