#ifndef CLK_HDR
#define CLK_HDR

#define CLOCK_BYTE_WIDTH 3

struct ticks {	
	uint8_t count[CLOCK_BYTE_WIDTH]; /* Max index is LSB */
};

void clock_init (void);
void clock_increment (void);
void clock_get_ticks (struct ticks *dst); /* Do not call in an interrupt context */

#endif 
// CLK_HDR