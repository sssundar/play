#include "twi_driver.h"

eStatus twi_init(sTWIDriver *twi) { (void) twi; return kfailure; }
void twi_deinit(sTWIDriver *twi) { (void) twi; return; }
eStatus twi_get(sTWIDriver* restrict twi, uint8_t addr, uint8_t reg, uint8_t* restrict dest) { (void) twi; (void) addr; (void) reg; (void) dest; return kerror; }
eStatus twi_put(sTWIDriver *twi, uint8_t addr, uint8_t reg, uint8_t src) { (void) twi; (void) addr; (void) reg; (void) src; return kerror; } 
