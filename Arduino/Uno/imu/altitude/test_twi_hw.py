# @file test_uart_hw.py
# @author Sushant Sundaresh
# @date 2017-01-09
# @details 
#  Intended as a companion script for test_twi_hw.py
# @usage
#  You must power cycle or reset the hardware system prior to executing 
#  this script for states between the Arduino & this script to be in sync.

import serial, struct, sys
from time import sleep

# Python Constants
NUM_BYTES_PER_SAMPLE = 9

if __name__ == "__main__":
	with serial.Serial(	port='/dev/ttyACM0', baudrate=9600, \
						bytesize=serial.EIGHTBITS, \
						parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE, 
						timeout=0.1) as uno:						
		sleep(2) 		# Anything less than or equal to 1 second seems 
		uno.write('s') 	# to lead to this 's' being ignored by the Uno
						
		data = [struct.unpack('B',x)[0] for x in uno.read(size=NUM_BYTES_PER_SAMPLE)]			
		print data
		sys.exit(0)			
