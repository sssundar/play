#include "bmp180_driver.h"

eStatus bmp180_init(sBMP180Driver* restrict barometer) {
    barometer->twi_address = BMP180_TWI_SLAVE_ADDRESS;
    barometer->is_sampling = kfalse;
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

static eStatus bmp180_start_sampling_(sBMP180Driver*restrict barometer,
                                    sTWIDriver* restrict twi,
                                    uint8_t control_register_value,
                                    uint8_t remaining_200Hz_ticks_till_sample_is_ready) {
    if (barometer->is_sampling == ktrue) { return kerror; }

    uint8_t control_register_address = BMP180_CTRL_REGISTER;    
    if (ksuccess != twi_tx_start(twi, 0))                                   { return kerror; }
    if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 0))             { return kerror; }    
    if (ksuccess != twi_tx_data(twi, &control_register_address, 1, kfalse)) { return kerror; }    
    if (ksuccess != twi_tx_start(twi, 1))                                   { return kerror; }    
    if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 0))             { return kerror; }
    if (ksuccess != twi_tx_data(twi, &control_register_value, 1, ktrue))    { return kerror; }
    barometer->remaining_200Hz_ticks_till_temperature_is_ready = remaining_200Hz_ticks_till_sample_is_ready; 
    barometer->is_sampling = ktrue;    
    return ksuccess;         
}

static eStatus bmp180_is_sample_ready_(sBMP180Driver* restrict barometer,                                 
                                        sTWIDriver* restrict twi) {
    if (barometer->is_sampling == kfalse) { return kfalse; }

    if (barometer->remaining_200Hz_ticks_till_temperature_is_ready > 0) {
        barometer->remaining_200Hz_ticks_till_temperature_is_ready -= 1;
    } else {
        uint8_t control_register_address = BMP180_CTRL_REGISTER;
        uint8_t control_register_value = 0;
        if (ksuccess != twi_tx_start(twi, 0))                                   { return kerror; }        
        if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 0))             { return kerror; }            
        if (ksuccess != twi_tx_data(twi, &control_register_address, 1, kfalse)) { return kerror; }            
        if (ksuccess != twi_tx_start(twi, 1))                                   { return kerror; }            
        if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 1))             { return kerror; }        
        if (ksuccess != twi_rx_data(twi, &control_register_value, 1))           { return kerror; }            
        if ((control_register_value & BMP180_CTRL_SCO_MASK) == 0) {             
            barometer->is_sampling = kfalse; 
            return ktrue; 
        } 
    }    

    return kfalse;
}

static eStatus bmp180_get_sample_data_(sBMP180Driver* restrict barometer, 
                                        sTimerDriver* restrict timer,
                                        sTWIDriver* restrict twi, 
                                        sData* restrict data,
                                        uint8_t data_address,
                                        eDataType data_type) {
    uint8_t data_values[2] = {0}; // Currently, the driver does not support 
                                  // oversampling pressure, so neither temperature and pressure
                                  // require more than two bytes
    sTicks timestamp;            

    if (ksuccess != twi_tx_start(twi, 0))                        { return kerror; }
    if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 0))  { return kerror; }    
    if (ksuccess != twi_tx_data(twi, &data_address, 1, kfalse))  { return kerror; }    
    if (ksuccess != twi_tx_start(twi, 1))                        { return kerror; }    
    if (ksuccess != twi_tx_sla(twi, barometer->twi_address, 1))  { return kerror; }
    if (ksuccess != twi_rx_data(twi, (uint8_t *) &data_values, 2)) { return kerror; }

    data_set_type(data, data_type);
    timer_get_ticks (timer, &timestamp); 
    data_set_ticks(data, &timestamp); 
    data_add_byte(data, data_values[0]);
    data_add_byte(data, data_values[1]);
    return ksuccess;
}

eStatus bmp180_start_pressure_sampling(sBMP180Driver* restrict barometer, 
                                        sTWIDriver* restrict twi) {
    return bmp180_start_sampling_(barometer, twi, BMP180_CTRL_PRESSURE, BMP180_200HZ_TICKS_PER_CONVERSION);
}

eStatus bmp180_is_pressure_ready(sBMP180Driver* restrict barometer,                                 
                                sTWIDriver* restrict twi) {
    return bmp180_is_sample_ready_(barometer,twi);
}

eStatus bmp180_get_pressure_data(sBMP180Driver* restrict barometer, 
                                sTimerDriver* restrict timer,
                                sTWIDriver* restrict twi, 
                                sData* restrict data) {
    return bmp180_get_sample_data_(barometer, timer, twi, data, BMP180_DATA_MSB_REGISTER, kdatatype_bmp180_barometry);
}

eStatus bmp180_start_temperature_sampling(sBMP180Driver* restrict barometer, 
                                        sTWIDriver* restrict twi) {
    return bmp180_start_sampling_(barometer, twi, BMP180_CTRL_TEMPERATURE, BMP180_200HZ_TICKS_PER_CONVERSION);
}

eStatus bmp180_is_temperature_ready(sBMP180Driver* restrict barometer,                                     
                                    sTWIDriver* restrict twi) {
    return bmp180_is_sample_ready_(barometer,twi);
}

eStatus bmp180_get_temperature_data(sBMP180Driver* restrict barometer, 
                                    sTimerDriver* restrict timer,
                                    sTWIDriver* restrict twi, 
                                    sData* restrict data) {
    return bmp180_get_sample_data_(barometer, timer, twi, data, BMP180_DATA_MSB_REGISTER, kdatatype_bmp180_thermometry);
}

