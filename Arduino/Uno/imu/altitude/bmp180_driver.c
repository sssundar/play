#include "bmp180_driver.h"

eStatus bmp180_init(sBMP180Driver* restrict barometer) {
    barometer->twi_address = BMP180_TWI_SLAVE_ADDRESS;
    barometer->remaining_200Hz_ticks_till_pressure_is_ready = 0;
    barometer->remaining_200Hz_ticks_till_temperature_is_ready = 0;      
    return ksuccess;  
}

eStatus bmp180_get_calibration(sBMP180Driver* restrict barometer, 
                                sTWIDriver* restrict twi, 
                                sUARTDriver* restrict uart,
                                sTimerDriver* restrict timer) {
    uint8_t parameter_address = BMP180_FIRST_CALIBRATION_REGISTER_ADDRESS;
    uint8_t num_parameters = BMP180_NUM_CALIBRATION_COEFFICIENTS;    
    uint8_t parameter_index = 0;
    for (parameter_index = 0; parameter_index < num_parameters; parameter_index++) {
        sData parameter;
        sTicks timestamp;
        uint8_t parameter_bytes[2]; 
        if (ksuccess != twi_tx_start(twi, 0))                              { return kerror; }
        if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 0))        { return kerror; }    
        if (ksuccess != twi_tx_data(twi, &parameter_address, 1, kfalse))   { return kerror; }    
        if (ksuccess != twi_tx_start(twi, 1))                              { return kerror; }    
        if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 1))        { return kerror; }
        if (ksuccess != twi_rx_data(twi, (uint8_t *) &parameter_bytes, 2)) { return kerror; }
        data_set_type(&parameter, kdatatype_bmp180_calibration);
        timer_get_ticks(timer, &timestamp);
        data_set_ticks(&parameter, &timestamp);
        data_add_byte(&parameter, parameter_bytes[0]);
        data_add_byte(&parameter, parameter_bytes[1]);
        data_add_byte(&parameter, parameter_address);
        uart_log(uart, &parameter);
        parameter_address += 2;
    }
    return ksuccess;
}

eStatus bmp180_start_pressure_sampling(sBMP180Driver* restrict barometer, 
                                        sTWIDriver* restrict twi) {
    (void) barometer; 
    (void) twi;
    return ksuccess;
}

eStatus bmp180_is_pressure_ready(sBMP180Driver* restrict barometer, 
                                sTimerDriver* restrict timer, 
                                sTWIDriver* restrict twi) {
    (void) barometer; 
    (void) twi;
    (void) timer;
    return ksuccess;
}

eStatus bmp180_get_pressure_data(sBMP180Driver* restrict barometer, 
                                sTimerDriver* restrict timer,
                                sTWIDriver* restrict twi, 
                                sData* restrict data) {
    (void) barometer; 
    (void) twi;
    (void) data;
    return ksuccess;
}

eStatus bmp180_start_temperature_sampling(sBMP180Driver* restrict barometer, 
                                        sTWIDriver* restrict twi) {
    (void) barometer; 
    (void) twi;    
    return ksuccess;
}

eStatus bmp180_is_temperature_ready(sBMP180Driver* restrict barometer, 
                                    sTimerDriver* restrict timer, 
                                    sTWIDriver* restrict twi) {
    (void) barometer; 
    (void) twi;
    (void) timer;
    return ksuccess;
}

eStatus bmp180_get_temperature_data(sBMP180Driver* restrict barometer, 
                                    sTimerDriver* restrict timer,
                                    sTWIDriver* restrict twi, 
                                    sData* restrict data) {
    (void) barometer; 
    (void) twi;
    (void) data;
    return ksuccess;
}
