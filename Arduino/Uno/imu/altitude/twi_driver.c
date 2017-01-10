#include "twi_driver.h"

/* Public Functions */

eStatus twi_init(sTWIDriver *twi) {
    if (twi == NULL) { return kerror; }    
    twi->status = 0;
#if AVR
    TWSR = 0; // Prescaler 1
    TWBR = 0x4A; // 100 kHz base clock off 16 MHz system clock / (16+2*74*1)            
#endif    
    return ksuccess;
}

void twi_deinit(sTWIDriver *twi) {
    if (twi == NULL) { return; }        
    twi->status = 0;    
#if AVR
    TWCR = 0;
#endif        
}

eStatus twi_tx_start (sTWIDriver *twi, uint8_t is_repeated) {    
    if (twi == NULL) { return kerror; }                
#if AVR    
    TWCR = TWCR_BASE | TWCR_TWSTA;        
#endif    
    while ((TWCR & TWCR_TWINT) == 0) {}
    twi->status = TWSR & TWSR_STATUS;
    uint8_t expected_status = is_repeated ? MASTER_REPEATED_START_TRANSMITTED : MASTER_START_TRANSMITTED;
    return (twi->status == expected_status) ? ksuccess : kerror;
}

eStatus twi_tx_sla (sTWIDriver *twi, uint8_t addr, uint8_t is_read) {
    if (twi == NULL) { return kerror; }            
#if AVR
    TWDR = addr + is_read;       
    TWCR = TWCR_BASE;
#endif    
    while ((TWCR & TWCR_TWINT) == 0) {}
    twi->status = TWSR & TWSR_STATUS;
    uint8_t expected_status = is_read ? MASTER_RX_SLAR_ACKD : MASTER_TX_SLAW_ACKD;
    return (twi->status == expected_status) ? ksuccess : kerror;
}

eStatus twi_tx_data (sTWIDriver* restrict twi, const uint8_t* restrict data, uint8_t num_bytes, eStatus stop_after_tx) {
    if (twi == NULL) { return kerror; }    
    if ((data == NULL) && (num_bytes > 0)) { return kerror; }
    if (num_bytes == 0) {         
        // Do not need to protect this against TWI interrupts since we know the bus won't be busy afterward.
        if (stop_after_tx == ktrue) {
            TWCR = TWCR_TWSTO | TWCR_BASE;
        }
        return ksuccess;
    }    
    uint8_t k = 0;
    uint8_t expected_status = MASTER_TX_DATA_ACKD;    
    for (k = 0; k < num_bytes; k++) {                
#if AVR
        TWDR = *(data+k);
        TWCR = TWCR_BASE;
#endif                
        while ((TWCR & TWCR_TWINT) == 0) {}
        twi->status = TWSR & TWSR_STATUS;
        if (twi->status != expected_status) {            
            return kerror;
        } 
    }
    if (stop_after_tx == ktrue) {
        TWCR = TWCR_TWSTO | TWCR_BASE;
    }
    return ksuccess;
}

eStatus twi_rx_data (sTWIDriver* restrict twi, uint8_t* restrict data, uint8_t num_bytes) {
    if (twi == NULL) { return kerror; }    
    if ((data == NULL) && (num_bytes > 0)) { return kerror; }
    if (num_bytes == 0) {         
        // Do not need to protect this against TWI interrupts since we know the bus won't be busy afterward.
        TWCR = TWCR_TWSTO | TWCR_BASE;
        return ksuccess;
    }    
    uint8_t k = 0;
    uint8_t expected_status = MASTER_RX_DATA_ACKD;    
    for (k = 0; k < (num_bytes-1); k++) {                
#if AVR        
        TWCR = TWCR_TWEA | TWCR_BASE;        
#endif                    
        while ((TWCR & TWCR_TWINT) == 0) {}
        twi->status = TWSR & TWSR_STATUS;
        if (twi->status != expected_status) {            
            return kerror;
        } 
        *(data+k) = TWDR;                    
    }
    expected_status = MASTER_RX_DATA_NACKD;            
    TWCR = TWCR_BASE;            
    while ((TWCR & TWCR_TWINT) == 0) {}
    twi->status = TWSR & TWSR_STATUS;
    if (twi->status != expected_status) {            
        return kerror;
    } 
    *(data+k) = TWDR;                                
    TWCR = TWCR_TWSTO | TWCR_BASE;
    return ksuccess;
}
