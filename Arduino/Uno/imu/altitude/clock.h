#ifndef CLK_HDR
#define CLK_HDR

#define GLOBAL_CLOCK_BYTE_WIDTH 3

struct ticks {	
	uint8_t count[GLOBAL_CLOCK_BYTE_WIDTH]; /* Max index is LSB */
}

struct ticks global_clock;

void init_global_clock (void);
void increment_global_ticks (void);
void get_global_ticks (struct ticks *dst);

#endif /* clock.h */