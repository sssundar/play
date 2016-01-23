/** 
  *  @now 	Verifying port of we_need_to_talk.s to C, here
  *  @next 	A 25 Hz Barometer & 1 Hz Temperature Sensor Stream to UART  
  *
  * @author Sushant Sundaresh
  * @lastrevised 22 January 2016
  * @build make all (with Uno connected)
  * 
  * @note avr-libc v1:1.8.0-4.1 source <avr/iom328p.h> from <avr/io.h> included
  */

#include <avr/io.h> 
#include <avr/interrupt.h>
#include "come_down_from_there.h"

/* Global Variables */
char CHAR_2_TRANSMIT = 'A';
volatile uint8_t TRANSMIT_FLAG; 	 	// See TX_COMPLETE, TX_STARTED


/* Timer 0 Oveflow Interrupt Handler Installation */
ISR(TIMER1_OVF_vect) { 	
									// check TRANSMIT_FLAG != TX_STARTED
	if (((signed char) TRANSMIT_FLAG) >= 0) {
		UDR0 = CHAR_2_TRANSMIT;	 	// queue up next UART transmission	
		TRANSMIT_FLAG = TX_SENT;
	}
}

/* USART0 Data Register Empty (Transmission) Interrupt Handler Installation */
ISR(USART_UDRE_vect) { 	
	TRANSMIT_FLAG = TX_DONE;
}

/* TWI Next-Action Interrupt Handler Installation 
ISR(TWI_vect) { 	

} */


void main(void) __attribute__((noreturn));
void main(void) { 

	/* Set up peripherals */ 	
	cli();
												// Set up USART0 for Tx 
	UCSR0A = TX_COMPLETE; 						// clear tx buffer flags			
	TRANSMIT_FLAG = TX_DONE;
	UCSR0B = TX_BUFF_READY_IE | TX_ENABLE; 		// interrupt on tx buffer ready		
	UCSR0C = USART_PARITY | TXRX_CHAR_SIZE10; 	// 8 bit characters, odd parity	
	UBRR0L = BAUD_ASYNCH_BASE_L;				// 115200 baud, asynch, 
	UBRR0H = BAUD_ASYNCH_BASE_H;	 			// no double-rate

										// Set up 1 Hz interrupt
	TCCR1A = 0x00; 						// normal mode	 					
 	TCCR1B = 0x04; 						// 256-prescaler off 16MHz base clock 	
 	TIMSK1 = 0x01;	  					// set up overflow interrupt 	

	/* TODO Set up separate queue structures for thermometer, barometer data */
 
	sei();


	/* Busy Wait */
	while (1) {				
	}
}