/**  
  * @includefor come_down_from_there.c
  * @author Sushant Sundaresh
  * @lastrevised 22 January 2016
  */


/* Temporary Flags, to verify avr assembly->c port */
#define TX_DONE 0x00
#define TX_SENT 0xFF

/* Data Structures  */

/* Sensor: BST-BMP180-DS000-09 on Adafruit p/n 1604 */
typedef struct barometer_sample {
	char data_type;
	char barom_MSB;
	char barom_LSB;
	char barom_XLSB;
} barometer_sample;

/* Sensor: BST-BMP180-DS000-09 on Adafruit p/n 1604 */
typedef struct temperature_sample {
	char data_type;
	char therm_MSB;
	char therm_LSB;
} temperature_sample;

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
