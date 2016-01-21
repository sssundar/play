;;;;;;;;;;;;
; AVR Assembly to Transmit a Single Character via RS232 at 1 Hz
; Note: relies on Arduino Uno having a USB-RS232 bridge
; Sushant Sundaresh, 21 January 2016
;;;;;;;;;;;;

;;;;;;;;;;;;;
; Build Instructions
; make speak
;;;;;;;;;;;;;

;;;;;;;;;;;;
; Definitions
;;;;;;;;;;;;

; Arduino Uno
.device ATmega328p

; Interrupt Vector Table Addresses
.define RST_VECT 0x0000
.define T1OVF_VECT 0x001A 
.define USART0_TX_BUFFER_READY_VECT 0x0026

; Sleep Control Register Address
.define SMCR 0x53 					

; Timer Control Register Addresses
.define TCCR1A 0x0080
.define TCCR1B 0x0081
.define TCNT1L 0x0084
.define TCNT1H 0x0085
.define TIMSK1 0x006F 		

; Stack Pointer Address 
.define SPH 0x005E
.define SPL 0x005D

; ---------------------------------------------
; USART Initialization with LD/ST I/O Addresses
; ---------------------------------------------
; register 			address 	initialize to
.define UDR0 		0xC6 		
								; n/a
.define UCSR0A		0xC0		
								; TX_COMPLETE
.define UCSR0B 		0xC1 		
								; TX_BUFF_READY_IE | TX_ENABLE
.define UCSR0C 		0xC2		
								; USART_PARITY | TXRX_CHAR_SIZE10
.define UBRR0L 		0xC4		
								; BAUD_9600_ASYNCH_BASE_L
.define UBRR0H		0xC5		
								; BAUD_9600_ASYNCH_BASE_H

; USART0 Control Register Bit Masks
; ---------------------------------

; UCSR0A Bit Masks 	Notes
; -------------------------
.define RX_COMPLETE 0x80	
							; R   r1 = unread data in RX buffer. 0 = empty.
.define TX_COMPLETE 0x40	
							; R/W r1 = empty tx buffer; w1 = clear it. 
							; can gen. tx intpt
.define TX_BUFFER_EMPTY 0x20
							; R   r1 = tx buffer can be written
							; can gen (UDRIE) intpt
.define RX_FRAME_ERROR 	0x10
					; R   r1 = error. Valid until UDR0 read. w0 on UCSR0A write.
.define DATA_OVERRUN 	0x08
							; R   r1 = error. Valid until UDR0 read.
	
.define PARITY_ERROR 	0x04
							; R   r1 = error. Valid until UDR0 read.
	
.define DOUBLE_BAUD		0x02
					; R/W w0 for synchronous. only in async CAN w1. ignore now.
	
.define MULTI_PC_COMM 	0x01
							; R/W ignore now, write 0.
	

; UCSR0B Bit Masks 	Notes
; -------------------------
.define RX_COMPLETE_IE 		0x80
							; R/W w1 = interrupt on rx_complete
.define TX_COMPLETE_IE  0x40	
							; R/W w1 = interrupt on tx complete
.define TX_BUFF_READY_IE  0x20	
							; R/W w1 = interrupt on tx data register ready
.define RX_ENABLE 	0x10 		
							; R/W w0 = disable
.define TX_ENABLE	0x08 		
							; R/W w1 = enable
.define TXRX_CHAR_SIZE2		0x04
						; R/W bit 2 of 3, 1:0 in UCSR0C. make this 0 bit 8 bit
.define RX_BIT8		0x02			
							; R ignore
.define TX_BIT8 	0x01 		
							; R/W ignore
	

; UCSR0C Bit Masks 	Notes
; -------------------------
.define USART_MODE_SEL 		0xC0
							; R/W w0 for UART
	
.define USART_PARITY  		0x30
							; R/W w 11 = odd parity. 
	
.define TX_STOP_BITS 		0x08
							; R/W w0 = 1 stop bit
	
.define TXRX_CHAR_SIZE10	0x06
							; R/W w 1 to both for 8 bit.
	
.define USART_CLOCK_POL 	0x01
							; R/W w 0 for asynchronous mode

; UBRR0H 
; UBRR0L Baud Rate Selection. With U2Xn (DOUBLE_BAUD) = 0,
;	7 	6 	5 	4 	3 	2 	1 	0
;	R 	R 	R  	R 	R/W R/W R/W R/W
;	R/W R/W R/W R/W R/W R/W R/W R/W
; Want 9600 Baud at 16 MHz. Set to 103 = 64 + 32 + 4 + 2 + 1 
;	0 	0 	0 	0 	0 	0	0 	0
;	0 	1 	1 	0 	0 	1 	1 	1
.define BAUD_9600_ASYNCH_BASE_H 	0x00
.define BAUD_9600_ASYNCH_BASE_L 	0x67


;;;;;;;;;;;;;;
; Code Segment
;;;;;;;;;;;;;;

.list
.cseg 									; Code Segment Follows											

.org RST_VECT							; Start of Program Memory 										 									
JMP RESET 								; Reset Handler
						
.org T1OVF_VECT							; IVT position of 
JMP TIM1_OVF 							; Timer 1 Overflow Handler

.org USART0_TX_BUFFER_READY_VECT 		; IVT position of 
JMP USART_TX_RDY 						; USART TX Buffer Empty/Ready Handler

; Interrupt Handlers

RESET:		
										; disable interrupts
	CLI 								

										; bottom out stack pointer at 1K SRAM
										; (leave 1K for data segment)
										; (stack grows DOWN)
										; if modifying this, change .dseg, below																				
	LDI r16, 0x04
	STS SPH, r16
	LDI r16, 0xFF
	STS SPL, r16 						
										
										; Setup for low-power looping
	LDI r16, 0x01	 					; initialize as 0x01 
	STS SMCR, r16						; to enable idle-sleep

										; Set up USART0 for Tx 
	LDI r16, TX_COMPLETE 						; clear tx buffer flags
	STS UCSR0A, r16						
	LDI r16, TX_BUFF_READY_IE | TX_ENABLE 		; interrupt on tx buffer ready
	STS UCSR0B, r16						 		
	LDI r16, USART_PARITY | TXRX_CHAR_SIZE10 	; 8 bit characters, odd parity
	STS UCSR0C, r16						
	LDI r16, BAUD_9600_ASYNCH_BASE_L	; 9600 baud, asynch, no double-rate
	STS UBRR0L, r16						
	LDI r16, BAUD_9600_ASYNCH_BASE_H
	STS UBRR0H, r16						

										
										; Set up 1 Hz interrupt
	LDI r16, 0x00
	STS TCCR1A, r16 					; normal mode
	LDI r16, 0x04
 	STS TCCR1B, r16 					; 256-prescaler off 16MHz base clock
 	LDI r16, 0x01
 	STS TIMSK1, r16  					; set up overflow interrupt 	

 										; get address of global flag into Z reg
	LDI r30,low(TRANSMIT_FLAG) 			; Load Z register with low byte of addr
	LDI r31,high(TRANSMIT_FLAG) 		; Load Z register with high byte of addr
										; this program is so simple that
										; we might as well save Z for this.

 										; get addr of CHAR_2_TRANSMIT in Y reg 										
	LDI r28,low(CHAR_2_TRANSMIT) 		; Load Y register with low byte of addr
	LDI r29,high(CHAR_2_TRANSMIT) 		; Load Y register with high byte of addr
										; this program is so simple that
										; we might as well save Y for this.

	LDI r16, 'D'
	ST Y, r16 							; write 'D' to CHAR_2_TRANSMIT
										; silly, could keep in cseg with .DB										

										; enable interrupts
	SEI 								
	JMP MAINLOOP 						

USART_TX_RDY:
										; set global flag: tx completed										
	LDI r16, 0x01	
	ST Z, r16 							; Load 1 into TRANSMIT_FLAG byte	
	RETI

TIM1_OVF:		
										; check TRANSMIT_FLAG; can we transmit?
	LD r16, Z
	AND r16, r16 						; set ZF
	BREQ TIM1_OVF_DONE 					; Branch to Done, if TRANSMIT_FLAG = 0
										; else can transmit again
	
	LD r16, Y 							; get character to transmit
	STS UDR0, r16 						; write it to tx buffer

										; clear global flag: tx started	
	EOR r16, r16 	
	ST Z, r16 							; Load 0 into TRANSMIT_FLAG byte

TIM1_OVF_DONE:
	RETI

; Main Loop
MAINLOOP:
	SLEEP 								; low-power spinner
	JMP MAINLOOP 						


;;;;;;;;;;;;;;
; Data Segment 
;;;;;;;;;;;;;;

.dseg
.org 0x0500 							; right after the stack base

TRANSMIT_FLAG: .BYTE 1 					; boolean. 0 = tx started
										; 		   1 = tx completed

CHAR_2_TRANSMIT: .BYTE  1				; set to ascii 'D'
										; make this in dseg, not cseg,
										; so that we can extend it to 
										; a data structure later for burst
										; transmitting