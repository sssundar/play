/**
  * @file data.h
  * @author Sushant Sundaresh
  * @date 2016-12-04
  * @details 
  * A data sample accessor helper. Defines what a data type is, and how
  * to set its type, bytes, and timestamp. 
  */

#ifndef _BMP180_DRIVER_H_
#define _BMP180_DRIVER_H_

#include "stdmath.h"
#include "types.h"
#include "timer_driver.h"
#include "twi_driver.h"
#include "uart_driver.h"
#include "data.h"

#define BMP180_TWI_SLAVE_ADDRESS 0xEE

#define BMP180_NUM_CALIBRATION_COEFFICIENTS         11
#define BMP180_FIRST_CALIBRATION_REGISTER_ADDRESS 0xAA

#define BMP180_DATA_MSB_REGISTER 0xF6
#define BMP180_DATA_LSB_REGISTER 0xF7
#define BMP180_DATA_XLSB_REGISTER 0xF8

#define BMP180_CTRL_REGISTER 0xF4
#define BMP180_CTRL_SCO_MASK 0x20   // Start of conversion (low when complete)

#define BMP180_CTRL_PRESSURE    0x34  // OSS 0, 4.5 ms
#define BMP180_CTRL_TEMPERATURE 0x2E  // 4.5 ms
#define BMP180_200HZ_TICKS_PER_CONVERSION 1     // For OSS 0 Pressure & Temperature

typedef struct sBMP180Driver {
    uint8_t twi_address;
    eStatus is_sampling; 
    int8_t remaining_200Hz_ticks_till_pressure_is_ready;
    int8_t remaining_200Hz_ticks_till_temperature_is_ready;
} sBMP180Driver;

// @details
// Sets up internal pressure/temperature sample timer tick counters.
// @param [in] barometer an uninitialized bmp180 driver object
// @return #ksuccess if successfully initialized driver
eStatus bmp180_init(sBMP180Driver* restrict barometer);

// @details
// Blocking, gets calibration data via TWI then transmits calibration via UART.
// @param [in] barometer a bmp180 driver object
// @param [in] twi a twi driver object
// @param [in] uart a uart driver object
// @param [in] timer a timer driver object
// @return #kerror: unsuccessful
// @return #ksuccess: otherwise
eStatus bmp180_get_calibration(sBMP180Driver* restrict barometer, 
                                sTWIDriver* restrict twi, 
                                sUARTDriver* restrict uart,
                                sTimerDriver* restrict timer);

// @details
// Starts pressure sampling via a TWI transmission and resets pressure sampling time counter
// @param [in] barometer a bmp180 driver object
// @param [in] twi a twi driver object
// @return #kerror: unsuccessful
// @return #ksuccess: otherwise
eStatus bmp180_start_pressure_sampling(sBMP180Driver* restrict barometer, 
                                        sTWIDriver* restrict twi);

// @details
// If called at 200Hz, decrements internal counter of ticks-till-sample-is-ready
// and, when this reaches zero, checks whether pressure data is ready. 
// @param [in] barometer a bmp180 driver object
// @param [in] twi a twi driver object
// @return #kfalse: no
// @return #kerror: twi error
// @return #ktrue: yes
eStatus bmp180_is_pressure_ready(sBMP180Driver* restrict barometer,                                 
                                sTWIDriver* restrict twi);

// @details
// Gets the pressure data and stores it, timestamped at the moment of retrieval, in the sData structure provided
// @param [in] barometer a bmp180 driver object
// @param [in] twi a twi driver object
// @param [in] timer a timer driver object
// @param [in] data where to save the pressure data
// @return #kerror: unsuccessful
// @return #ksuccess: otherwise
eStatus bmp180_get_pressure_data(sBMP180Driver* restrict barometer, 
                                sTimerDriver* restrict timer,
                                sTWIDriver* restrict twi, 
                                sData* restrict data);

// @details
// Starts temperature sampling via a TWI transmission and resets temperature sampling time counter
// It is expected that the caller polls bmp180_is_temperature_ready at 200Hz following
// this call.
// @param [in] barometer a bmp180 driver object
// @param [in] twi a twi driver object
// @return #kerror: unsuccessful
// @return #ksuccess: otherwise
eStatus bmp180_start_temperature_sampling(sBMP180Driver* restrict barometer, 
                                        sTWIDriver* restrict twi);

// @details
// If called at 200Hz, decrements internal counter of ticks-till-sample-is-ready
// and, when this reaches zero, checks whether temperature data is ready. 
// @param [in] barometer a bmp180 driver object
// @param [in] twi a twi driver object
// @return #kfalse: no
// @return #kerror: twi error
// @return #ktrue: yes
eStatus bmp180_is_temperature_ready(sBMP180Driver* restrict barometer,                                     
                                    sTWIDriver* restrict twi);

// @details
// Gets the temperature data and stores it, timestamped at the moment of retrieval, in the sData structure provided
// @param [in] barometer a bmp180 driver object
// @param [in] timer a timer driver object
// @param [in] twi a twi driver object
// @param [in] data where to save the temperature data
eStatus bmp180_get_temperature_data(sBMP180Driver* restrict barometer, 
                                    sTimerDriver* restrict timer,
                                    sTWIDriver* restrict twi, 
                                    sData* restrict data);

#endif 
// _BMP180_DRIVER_H_   
