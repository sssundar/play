/**
  * @file data.h
  * @author Sushant Sundaresh
  * @date 2016-12-04
  * @details 
  * A data sample accessor helper. Defines what a data type is, and how
  * to set its type, bytes, and timestamp. 
  */

#ifndef _DATA_H_
#define _DATA_H_

#include "types.h"
#include "stdmath.h"
#include "timer_driver.h"

/* Enumeration of supported data types. Must fit in uint8_t. */
typedef enum eDataType {
    kdatatype_unset,
    kdatatype_bmp180_calibration,
    kdatatype_bmp180_barometry,
    kdatatype_bmp180_thermometry    
} eDataType;

/* Byte width of supported data types */
#define BYTEWIDTH_BMP180_CALIBRATION    3 // Transmitted as follows: MSB parameter-MSB | parameter-LSB | EEPROM address LSB, 
                                          // where the calibration bytes are guaranteed not to be 0xFF or 0x00
#define BYTEWIDTH_BMP180_BAROMETRY      3 
#define BYTEWIDTH_BMP180_THERMOMETRY    2
#define MAX_DATA_BYTES                  3 

/* A data sample */
typedef struct sData  {
    sTicks timestamp;               /* Acquisition time in ticks, estimate */
    uint8_t bytes[MAX_DATA_BYTES];  /* Data sample bytes, MSB (0) .. LSB (MAX_DATA_BYTES-1) */ 
    uint8_t num_bytes;              /* The number of bytes currently saved in this sample */
    uint8_t max_bytes;              /* The maximum number of data bytes for this data type */
    eDataType type;                 /* Type of sample */        
} sData;

/* Accessors */

/** 
  * @details
  * Sets the new type of the data sample and clears the bytes in this data sample 
  * as this function should only be called when initializing a new data sample.
  * @param [in] data a data sample
  * @param [in] type the type of data
  * @return #kerror: null pointer as input
  * @return #ksuccess: successful
  */
eStatus data_set_type(sData *data, eDataType type);

/** 
  * @details
  * Grabs the type of the data sample.
  * @param [in] data a data sample
  * @param [in] type the type of the data
  * @return #kerror: null pointers as inputs
  * @return #ksuccess: successful
  */
eStatus data_get_type(const sData* restrict data, eDataType* restrict type);

/** 
  * @details
  * Write data byte to a data sample. 
  * @param [in] data a data sample
  * @param [in] byte the byte to add  
  * @return #kerror: null pointer as input or data type not yet set  
  * @return #ksuccess: successfully wrote byte
  */
eStatus data_add_byte(sData *data, uint8_t byte);

/** 
  * @details
  * Copies the data bytes of a data sample.
  * @param [in] data a data sample
  * @param [in] bytes MAX_DATA_BYTES length buffer to copy the bytes into
  * @param [in] len the number of bytes copied over
  * @return #kerror: null pointers as inputs
  * @return #ksuccess: successfully copied data bytes
  */
eStatus data_get_bytes(const sData* restrict data, uint8_t* restrict bytes, uint8_t* restrict len);

/** 
  * @details
  * Timestamps a data sample. 
  * @param [in] data a data sample
  * @param [in] ticks a timestamp in timer ticks
  * @return #kerror: null pointers as inputs
  * @return #ksuccess: successfully timestamped data.
  */
eStatus data_set_ticks(sData* restrict data, const sTicks* restrict ticks);

/** 
  * @details
  * Gets the timestamp of a data sample. 
  * @param [in] data a data sample
  * @param [in] ticks where to write the timestamp
  * @return #kerror: null pointers as inputs
  * @return #ksuccess: successfully copied timestamp of data.
  */
eStatus data_get_ticks(const sData* restrict data, sTicks* restrict ticks); 

/**
  * @details
  * Copies one data sample to another.
  * @todo This could be much more efficient if you didn't use the public API
  * @param [in] src the source
  * @param [in] dst the destinatoon
  * @return #kerror: null pointers as inputs
  * @return #ksuccess: successful
  */
eStatus data_copy(const sData* restrict src, sData* restrict dst);

#endif 
// _DATA_H_