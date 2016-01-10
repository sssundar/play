;;;;;;;;;;;;
; First Pass, AVR Assembly to Blink an LED, using AVRA & AVR-DUDE
; Sushant Sundaresh, 9 January 2016
;;;;;;;;;;;;

;;;;;;;;;;;;;
; AVRA Command Line Arguments
; avra blinkLED.s
; AVRDUDE Command Line Arguments
; avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:blinkLED.s.hex 
;;;;;;;;;;;;;

;;;;;;;;;;;;
; Definitions
;;;;;;;;;;;;

; Arduino Uno
.device ATmega328p

; Interrupt Vector Table Addresses
.define RST_VECT 0x0000
.define T1OVF_VECT 0x001A 

; Timer Control Register Addresses
.define TCCR1A 0x0080
.define TCCR1B 0x0081
.define TCNT1L 0x0084
.define TCNT1H 0x0085
.define TIMSK1 0x006F 		

; IO Pin Control Registers (in IO space, using IN/OUT)
.define DDRB 0x0004
.define PORTB 0x0005
.define PINB 0x0003

; Stack Pointer Address 
.define SPH 0x005E
.define SPL 0x005D

;;;;;;;;;;;;
; Program
;;;;;;;;;;;;

.list
.cseg 									; Code Segment Follows											

.org RST_VECT							; Start of Program Memory 										 									
JMP RESET 								; Reset Handler
						
.org T1OVF_VECT							; IVT position of 
JMP TIM1_OVF 							; Timer 1 Overflow Handler


TIM1_OVF:	
	SBI PINB, 0							; toggle PB0 between source and sink	
	RETI


RESET:		
	CLI 								; disable interrupts

	LDI r16, 0x08
	STS SPH, r16
	LDI r16, 0xFF
	STS SPL, r16 						; bottom out stack pointer at 2K SRAM

										; set PB0 low (LED off)
	SBI DDRB, 0 						; set PB0 to an output
	CBI PORTB, 0 						; set PB0 to a sink

										; Set up 1 Hz interrupt
	LDI r16, 0x00
	STS TCCR1A, r16 					; normal mode
	LDI r16, 0x04
 	STS TCCR1B, r16 					; 256-prescaler off 16MHz base clock
 	LDI r16, 0x01
 	STS TIMSK1, r16  					; set up overflow interrupt 	

	SEI 								; enable interrupts

MAINLOOP:
	JMP MAINLOOP 						; loop forever, toggling LED at 1 Hz