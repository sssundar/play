#include "clock.h"
#include "data.h"

void set_sample_type(struct data *dst, uint8_t type) {
	dst->type = type;
}

void set_sample_time(struct data *dst, struct ticks *src) {		
	struct ticks *d = &(dst->c);
	for (uint8_t i = 0; i < CLOCK_BYTE_WIDTH; i++) {
		d->count[i] = src->count[i];
	}		
}

void set_sample_bytes(struct data *dst, struct sample *src) {	
	struct sample *d = &(dst->s);
	for (uint8_t i = 0; i < MAX_DATA_BYTES; i++) {
		d->bytes[i] = src->bytes[i];
	}	
}

void get_sample_type(uint8_t *dst, struct data *src) {
	*dst = src->type;
}

void get_sample_time(struct ticks *dst, struct data *src) {		
	struct ticks *tsrc = &(src->c);
	for (uint8_t i = 0; i < CLOCK_BYTE_WIDTH; i++) {
		dst->count[i] = tsrc->count[i];
	}		
}

void get_sample_bytes(struct sample *dst, struct data *src) {	
	struct sample *ssrc = &(src->s);
	for (uint8_t i = 0; i < MAX_DATA_BYTES; i++) {
		dst->bytes[i] = ssrc->bytes[i];
	}	
}