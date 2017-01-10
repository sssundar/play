#include "twi_driver.h"

/* Local Functions */

// @details
// Called from the TWI ISR. Gets the TWI peripheral status and sets the is_busy 
// flag in the twi object to 0 (false).
// @param [in] twi a TWI driver object, uninitialized
static void twi_get_status_and_indicate_peripheral_is_no_longer_busy_ (sTWIDriver *twi) {
    twi->status = TWSR & TWSR_STATUS;
    twi->is_busy = 0;
}

/* Interrupt Service Routine */

#if AVR
/**
  * @details
  * Called after each TWI operation completes. 
  */
ISR(TWI_vect) {                        
    twi_get_status_and_indicate_peripheral_is_no_longer_busy_(&twi);
}
#endif

/* Public Functions */

eStatus twi_init(sTWIDriver *twi) {
    if (twi == NULL) { return kerror; }
    _PROTECT(kinterrupt_save_flags);     
    twi->is_busy = 0;
    twi->status = 0;
#if AVR
    TWSR = 0; // Prescaler 1
    TWBR = 0; // 1 MHz base clock for TWI with prescaler 1    
#endif
    _RELEASE(kinterrupt_restore_flags);
    return ksuccess;
}

void twi_deinit(sTWIDriver *twi) {
    if (twi == NULL) { return; }
    _PROTECT(kinterrupt_save_flags);     
    twi->is_busy = 0;
    twi->status = 0;    
#if AVR
    TWCR = 0;
#endif    
    _RELEASE(kinterrupt_restore_flags);
}

eStatus twi_tx_start (sTWIDriver *twi, uint8_t is_repeated) {
    if (twi == NULL) { return kerror; }    
    _PROTECT(kinterrupt_save_flags);   
#if AVR
    TWCR = TWCR_BASE | TWCR_TWSTA;
#endif
    twi->is_busy = 1;
    _RELEASE(kinterrupt_restore_flags);
    while (twi->is_busy) { _SLEEP(); }
    uint8_t expected_status = is_repeated ? MASTER_REPEATED_START_TRANSMITTED : MASTER_START_TRANSMITTED;
    return (twi->status == expected_status) ? ksuccess : kerror;
}

eStatus twi_tx_sla (sTWIDriver *twi, uint8_t addr, uint8_t is_write) {
    if (twi == NULL) { return kerror; }    
    _PROTECT(kinterrupt_save_flags);   
#if AVR
    TWDR = (addr | 0x01) & ~is_write;
    TWCR = TWCR_BASE;
#endif
    twi->is_busy = 1;
    _RELEASE(kinterrupt_restore_flags);
    while (twi->is_busy) { _SLEEP(); }
    uint8_t expected_status = is_write ? MASTER_TX_SLAW_ACKD : MASTER_RX_SLAR_ACKD;
    return (twi->status == expected_status) ? ksuccess : kerror;
}

eStatus twi_tx_data (sTWIDriver* restrict twi, const uint8_t* restrict data, uint8_t num_bytes, eStatus stop_after_tx) {
    if (twi == NULL) { return kerror; }    
    if ((data == NULL) && (num_bytes > 0)) { return kerror; }
    if (num_bytes == 0) {         
        // Do not need to protect this against TWI interrupts since we know the bus won't be busy afterward.
        TWCR = TWCR_TWSTO | TWCR_BASE;
        return ksuccess;
    }    
    uint8_t k = 0;
    uint8_t expected_status = MASTER_TX_DATA_ACKD;    
    for (k = 0; k < num_bytes; k++) {
        _PROTECT(kinterrupt_save_flags);       
#if AVR
        TWDR = *(data+k);
        TWCR = TWCR_BASE;
#endif        
        twi->is_busy = 1;
        _RELEASE(kinterrupt_restore_flags);                    
        while (twi->is_busy) { _SLEEP(); }
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
        _PROTECT(kinterrupt_save_flags);       
#if AVR        
        TWCR = TWCR_TWEA | TWCR_BASE;        
#endif        
        twi->is_busy = 1;
        _RELEASE(kinterrupt_restore_flags);                    
        while (twi->is_busy) { _SLEEP(); }
        if (twi->status != expected_status) {            
            return kerror;
        } 
        *(data+k) = TWDR;                    
    }
    expected_status = MASTER_RX_DATA_NACKD;    
    _PROTECT(kinterrupt_save_flags);       
    TWCR = TWCR_BASE;    
    twi->is_busy = 1;    
    _RELEASE(kinterrupt_restore_flags);                    
    while (twi->is_busy) { _SLEEP(); }
    if (twi->status != expected_status) {            
        return kerror;
    } 
    *(data+k) = TWDR;                                
    TWCR = TWCR_TWSTO | TWCR_BASE;
    return ksuccess;
}
