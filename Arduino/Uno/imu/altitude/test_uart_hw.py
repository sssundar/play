# @file test_uart_hw.py
# @author Sushant Sundaresh
# @date 2016-12-29
# @details 
#  Intended as a companion script for test_uart_hw.c 
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
		sleep(1.1) 		# Anything less than or equal to 1 second seems 
		uno.write('s') 	# to lead to this 's' being ignored by the Uno
		
		previous_timestamp = 0
		while True:									
			data = [struct.unpack('B',x)[0] for x in uno.read(size=NUM_BYTES_PER_SAMPLE)]			
			
			###########################################
			# Print the timestamp to verify frequency #
			###########################################
			next_timestamp = int(data[0]) * 256 * 256 + int(data[1]) * 256 + int(data[2])			
			if not (next_timestamp - previous_timestamp == 10):
				# UART transmissions are not arriving at 20 Hz (10 ticks apart at 200 Hz base clock)
				# This is a critical error				
				print "Invalid UART Transmission Frequency from Uno: %d ticks" % (next_timestamp-previous_timestamp)
				uno.write('e') 
				sys.exit(1) 
				break
			previous_timestamp = next_timestamp

			###################################################################################
			# Uncomment the following if you want to test byte packing for an sData structure #
			###################################################################################
			# print data
			# if (len(data) == NUM_BYTES_PER_SAMPLE):				
			# 	k = 1
			# 	success = True
			# 	while (k < 10):
			# 		success &= (data[k-1] == k)
			# 		k += 1
			# 	if (not success):
			# 		print "Invalid UART Transmission from Uno: "					
			#       uno.write('e') 
			# 		sys.exit(1)
			# 		break
