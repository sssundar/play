#include "config.h"
#include "io.h"

#if AVR == 0
uint8_t TCCR0A;
uint8_t TCCR0B;
uint8_t OCR0A;
uint8_t TIMSK0;
#endif
