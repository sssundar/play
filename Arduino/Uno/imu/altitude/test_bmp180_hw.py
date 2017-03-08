# @file test_bmp180_hw.py
# @author Sushant Sundaresh
# @date 2017-01-10
# @details 
#  Intended as a companion script for test_bmp180_hw.c
# @usage
#  You must power cycle or reset the hardware system prior to executing 
#  this script for states between the Arduino & this script to be in sync.

import serial, struct, sys
from time import sleep

# Python Constants
NUM_BYTES_PER_SAMPLE = 9

NUM_CALIBRATION_COEFFICIENTS = 11
BASE_COEFFICIENT_ADDRESS = 0xAA

KDATATYPE_BMP180_BAROMETRY = 2
KDATATYPE_BMP180_THERMOMETRY = 3

def set_up_coefficient_dict ():
	parameter_details = [["AC1", "short"], 
						["AC2", "short"], 
						["AC3", "short"], 
						["AC4", "unsigned short"], 
						["AC5", "unsigned short"], 
						["AC6", "unsigned short"], 
						["B1", "short"], 
						["B2", "short"], 
						["MB", "short"], 
						["MC", "short"], 
						["MD", "short"]]	
	parameter_address = BASE_COEFFICIENT_ADDRESS
	address_to_name_and_type = {}	
	for name in parameter_details:
		address_to_name_and_type[parameter_address] = name
		parameter_address += 2
	return address_to_name_and_type

def sign_u16_parameter (unsigned_value, signed):
	if signed:
		if (unsigned_value > ((2**15)-1)):
			return float(unsigned_value - 2**16)
	return float(unsigned_value)

if __name__ == "__main__":
	parameter_address_to_name_and_type = set_up_coefficient_dict()
	parameters = {}

	with serial.Serial(	port='/dev/ttyACM0', baudrate=9600, \
						bytesize=serial.EIGHTBITS, \
						parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE, 
						timeout=0.2) as uno:						
		sleep(2) 		# Anything less than or equal to 1 second seems 
		uno.write('s') 	# to lead to this 's' being ignored by the Uno
		
		# Grab the calibration coefficients and parse them
		for x in xrange(NUM_CALIBRATION_COEFFICIENTS):
			data = [struct.unpack('B',x)[0] for x in uno.read(size=NUM_BYTES_PER_SAMPLE)]						
			(name, sign_and_type) = parameter_address_to_name_and_type[data[5]]			
			parameters[name] = sign_u16_parameter(data[3] * 256 + data[4], sign_and_type == "short")		
		
		for k in sorted(parameters.keys()):
			print (k, parameters[k])

		# Parse the incoming, endless stream of temperature data	
		B5 = 4000.0
		while True:			
			data = [struct.unpack('B',x)[0] for x in uno.read(size=NUM_BYTES_PER_SAMPLE)]												
			timestamp = float(data[0] * 256 * 256 + data[1] * 256 + data[2]) * (1.0/200) # seconds
			if (data[8] == KDATATYPE_BMP180_THERMOMETRY):
				# We guarantee that this will occur before KDATATYPE_BMP180_BAROMETRY, at roughly 1 Hz
				# so B5 will be well defined when KDATATYPE_BMP180_BAROMETRY is processed
				UT = float(data[3] * 256 + data[4])
				X1 = ((UT-parameters["AC6"]) * parameters["AC5"]) / (2**15)
				X2 = (parameters["MC"] * 2**11) / (X1 + parameters["MD"])
				B5 = X1 + X2 
				temperature_c = ((B5+8)/16)/10 # temperature in celcius
				print (timestamp, temperature_c)
			elif (data[8] == KDATATYPE_BMP180_BAROMETRY):
				# Coming in at about 10 Hz 
				# 1 sample UT, then 9 samples UP, spaced roughly 100ms apart, currently
				UP = float(data[3] * 256 + data[4]) # no oversampling
				B6 = float(B5-4000)
				X1 = (parameters["B2"] * ((B6 * B6)/(4096)))/2048
				X2 = (parameters["AC2"] * B6)/2048
				X3 = X1 + X2
				B3 = ((parameters["AC1"]*4 + X3) + 2)/4
				X1 = (parameters["AC3"]*B6)/(2**13)
				X2 = (parameters["B1"] * ((B6 * B6)/4096))/(2**16)
				X3 = ((X1+X2) + 2)/4
				B4 = (parameters["AC4"] * (X3+(2**15)))/(2**15)
				B7 = (UP - B3) * 50000
				if (B7 < (2**31)): 	# no longer necessary
					p = (B7 * 2) / B4
				else:
					p = (B7/B4) * 2
				X1 = (p/256)**2
				X1 = (X1 * 3038) / (2**16)
				X2 = (-7357 * p) / (2**16)
				p += ((X1 + X2 + 3791)/16) # Pascals
				pressure_Pa = p
				print (timestamp, pressure_Pa)
			else:
				print
				print "###############"
				print "Received data that was not thermometry or barometry:"
				print data
				print "###############"
				print						

		sys.exit(0)			
