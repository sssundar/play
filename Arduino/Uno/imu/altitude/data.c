#include "data.h"

eStatus data_set_type(sData *data, eDataType type) {    
    if (data == NULL) { return kerror; }    
    data->num_bytes = 0;
    switch (type) {     
        case kdatatype_unset:                   
            data->max_bytes = 0;
            break;
        case kdatatype_bmp180_calibration:             
            data->max_bytes = BYTEWIDTH_BMP180_CALIBRATION;
            break;
        case kdatatype_bmp180_barometry:
            data->max_bytes = BYTEWIDTH_BMP180_BAROMETRY;
            break;
        case kdatatype_bmp180_thermometry:
            data->max_bytes = BYTEWIDTH_BMP180_THERMOMETRY;
            break;                  
    }
    data->type = type;
    return ksuccess;
}

eStatus data_get_type(const sData* restrict data, eDataType* restrict type) {
    if ((data == NULL) || (type == NULL)) { return kerror; }
    *type  = data->type;
    return ksuccess;
}

eStatus data_add_byte(sData *data, uint8_t byte) {
    if (data == NULL) { return kerror; }
    if (data->num_bytes >= data->max_bytes) { return kerror; }
    data->bytes[data->num_bytes] = byte;
    data->num_bytes += 1;
    return ksuccess;
}

eStatus data_get_bytes(const sData* restrict data, uint8_t* restrict bytes, uint8_t* restrict len) {
    if ((data == NULL) || (bytes == NULL) || (len == NULL)) { return kerror; }
    uint8_t k;
    *len = data->num_bytes;
    for (k = 0; k < data->num_bytes; k++) {
        *(bytes+k) = data->bytes[k];
    }
    return ksuccess;
}

eStatus data_set_ticks(sData* restrict data, const sTicks* restrict ticks) {
    if (data == NULL) { return kerror; }
    return timer_copy_ticks(ticks, &(data->timestamp));
}

eStatus data_get_ticks(const sData* restrict data, sTicks* restrict ticks) {
    if (data == NULL) { return kerror; }
    return timer_copy_ticks(&(data->timestamp), ticks);
}

eStatus data_copy(const sData* restrict src, sData* restrict dst) {
    if ((src == NULL) || (dst == NULL)) { return kerror; }
    
    eDataType dt;
    data_get_type(src, &dt);
    data_set_type(dst, dt);

    sTicks ticks;
    data_get_ticks(src, &ticks);
    data_set_ticks(dst, &ticks);

    uint8_t bytes[MAX_DATA_BYTES];
    uint8_t len;
    data_get_bytes(src, bytes, &len);
    uint8_t k;
    for (k = 0; k < len; k++) {
        data_add_byte(dst, bytes[k]);
    }

    return ksuccess;
}
