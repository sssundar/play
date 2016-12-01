/**
  * @file stdmath.h
  * @author Sushant Sundaresh
  * @date 2016-11-30  
  * @details
  * If the config.h header has AVR defined we
  * use the standard AVR definitions of math.h.
  * Otherwise we use stdint.h and its friends.
  */

#ifndef _STD_MATH_H_
#define _STD_MATH_H_

#include "config.h"
  
#if AVR
#include <math.h>  
#define NULL 0
#else
#include <stddef.h>
#include <stdint.h>
#endif

#endif
// _STD_MATH_H_
