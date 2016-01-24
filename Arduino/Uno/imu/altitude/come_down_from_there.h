/**  
  * @includefor come_down_from_there.c
  * @author Sushant Sundaresh
  * @lastrevised 23 January 2016
  *
  * @unittest test_queue_accessors.c, passed 23 January 2016
  */

/* Temporary Flags, to verify avr assembly->c port */
#define TX_DONE 0x00
#define TX_SENT 0xFF

/* Boolean Constants */
#define TRU 0x01
#define FLS 0x00

/* Data Structures  */

/* Data Type Definitions
	Keep low nibble as byte-storage counter
	for simpler post-processing */
#define BMP180_CALIBRATION 	0xF0 | 0x01 	// MSB
#define BAROMETER_DATA 		0xE0 | 0x03 	// MSB, LSB, XLSB at 8x oversampling
#define THERMOMETER_DATA	0xD0 | 0x02 	// MSB, LSB		

/* Sensor: BST-BMP180-DS000-09 on Adafruit p/n 1604 
	Data Types: 
		BMP180_CALIBRATION
		BAROMETER_DATA
		THERMOMETER_DATA
*/
typedef struct data {
	uint8_t type; 		
	uint8_t MSB; 	
	uint8_t LSB; 		
	uint8_t XLSB;
} data;

/* 	circular queue 
	length is fixed at 256 4 byte structs, or 1 kilobyte, half our SRAM 
	naturally addition is modulu 256 for uint8_t tail/head indices

	tail == head => empty
	head == tail + 1 % 256 => full 
	enqueuing: tail += 1 % 256, head = head, post increment
	dequeuing: head += 1 % 256, tail = tail; post decrement
	notice, at least 1 dead data element even when full.
*/
#define QUEUE_LENGTH 255
typedef struct circular_queue {
	uint8_t tail;
	uint8_t head;
	data buffer[QUEUE_LENGTH+1];
} circular_queue;

/* does not need to be interrupt safe - only called within handlers */
static uint8_t can_enqueue (circular_queue* q) {
	/* check if full */
	if ((q->tail + 1) == q->head) {
		return (uint8_t) FLS;
	}
	return (uint8_t) TRU;
}

/* does not need to be interrupt safe - only called within handlers */
static uint8_t can_dequeue	(circular_queue* q) {
	/* check if empty */
	if (q->tail == q->head) {
		return (uint8_t) FLS;
	}
	return (uint8_t) TRU;	
}

/* 	must call can_enqueue first 
	does not need to be interrupt safe - only called within handlers */
static void enqueue (data* sample, circular_queue* q) {
	data* record = &(q->buffer[q->tail]);
	record->type = sample->type;
	record->MSB = sample->MSB;
	record->LSB = sample->LSB;
	record->XLSB = sample->XLSB;
	q->tail += 1;
}

/* 	must call can_dequeue first 
	does not need to be interrupt safe - only called within handlers */
static void dequeue (data* sample, circular_queue* q) {
	data* record = &(q->buffer[q->head]);
	sample->type = record->type;
	sample->MSB = record->MSB;
	sample->LSB = record->LSB;
	sample->XLSB = record->XLSB;
	q->head += 1;
}

/* Timer Control Register io328p.h register names
	TCCR1A, TCCR1B, TCNT1L, TCNT1H, TIMSK1 */

/* 	---------------------------------------------
	USART Initialization with LD/ST I/O Addresses
	---------------------------------------------
	io328p.h register names
 		initialize to
UDR0 	n/a								
UCSR0A  TX_COMPLETE
UCSR0B 	TX_BUFF_READY_IE | TX_ENABLE
UCSR0C 	USART_PARITY | TXRX_CHAR_SIZE10
UBRR0L 	BAUD_9600_ASYNCH_BASE_L
UBRR0H	BAUD_9600_ASYNCH_BASE_H
*/

/* USART0 Control Register Bit Masks */

/* UCSR0A Bit Masks 		Notes */
#define RX_COMPLETE 0x80	
							// R   r1 = unread data in RX buffer. 0 = empty.
#define TX_COMPLETE 0x40	
							// R/W r1 = empty tx buffer// w1 = clear it. 
							// can gen. tx intpt
#define TX_BUFFER_EMPTY 0x20
							// R   r1 = tx buffer can be written
							// can gen (UDRIE) intpt
#define RX_FRAME_ERROR 	0x10
					// R   r1 = error. Valid until UDR0 read. w0 on UCSR0A write.
#define DATA_OVERRUN 	0x08
							// R   r1 = error. Valid until UDR0 read.
	
#define PARITY_ERROR 	0x04
							// R   r1 = error. Valid until UDR0 read.
	
#define DOUBLE_BAUD		0x02
					// R/W w0 for synchronous. only in async CAN w1. ignore now.
	
#define MULTI_PC_COMM 	0x01
							// R/W ignore now, write 0.
	

/* UCSR0B Bit Masks 	Notes */
#define RX_COMPLETE_IE 		0x80
							// R/W w1 = interrupt on rx_complete
#define TX_COMPLETE_IE  0x40	
							// R/W w1 = interrupt on tx complete
#define TX_BUFF_READY_IE  0x20	
							// R/W w1 = interrupt on tx data register ready
#define RX_ENABLE 	0x10 		
							// R/W w0 = disable
#define TX_ENABLE	0x08 		
							// R/W w1 = enable
#define TXRX_CHAR_SIZE2		0x04
						// R/W bit 2 of 3, 1:0 in UCSR0C. make this 0 bit 8 bit
#define RX_BIT8		0x02			
							// R ignore
#define TX_BIT8 	0x01 		
							// R/W ignore
	

/* UCSR0C Bit Masks 	Notes */
#define USART_MODE_SEL 		0xC0
							// R/W w0 for UART
	
#define USART_PARITY  		0x30
							// R/W w 11 = odd parity. 
	
#define TX_STOP_BITS 		0x08
							// R/W w0 = 1 stop bit
	
#define TXRX_CHAR_SIZE10	0x06
							// R/W w 1 to both for 8 bit.
	
#define USART_CLOCK_POL 	0x01
							// R/W w 0 for asynchronous mode

/* UBRR0H 
 	UBRR0L Baud Rate Selection. With U2Xn (DOUBLE_BAUD) = 0,
	7 	6 	5 	4 	3 	2 	1 	0
	R 	R 	R  	R 	R/W R/W R/W R/W
	R/W R/W R/W R/W R/W R/W R/W R/W
 Want 115200 Baud at 16 MHz. Set to 8
	0 	0 	0 	0 	0 	0	0 	0
	0 	0 	0 	0 	1	0	0 	0
*/
#define BAUD_ASYNCH_BASE_H 	0x00
#define BAUD_ASYNCH_BASE_L 	0x08
