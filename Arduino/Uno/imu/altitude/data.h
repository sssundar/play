#ifndef MEASUREMENT
#define MEASUREMENT

#include "clock.h"

/* Byte width of supported data types */
#define MAX_DATA_BYTES 3 
#define BYTEWIDTH_BMP180_CALIBRATION	1
#define BYTEWIDTH_BMP180_BAROMETRY 		3
#define BYTEWIDTH_BMP180_THERMOMETRY	2
struct sample {
	uint8_t bytes[MAX_DATA_BYTES]; /* Sample bytes */ 
};

/* Enumeration of supported data types. Must fit in uint8_t */
typedef enum {
	kbmp180_calibration = 0,
	kbmp180_barometry = 1,
	kbmp180_thermometry = 2
} DATATYPE;

struct data {
	uint8_t type; 		/* Type of sample */	
	struct ticks c; 	/* Acquisition time, estimate */
	struct sample s;	/* Sample bytes */
};

void set_sample_type(struct data *dst, uint8_t type);
void set_sample_time(struct data *dst, struct ticks *src);
void set_sample_bytes(struct data *dst, struct sample *src);
void get_sample_type(uint8_t *dst, struct data *src);
void get_sample_time(struct ticks *dst, struct data *src);
void get_sample_bytes(struct sample *dst, struct data *src);
#endif 
// MEASUREMENT