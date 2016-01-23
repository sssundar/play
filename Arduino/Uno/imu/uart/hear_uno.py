# hear_uno.py
# 
# Listens for Arduino Uno on /dev/ttyACM0 at 
#  9600 baud
#  odd parity
#  8 bits
#  1 stop bit
# Expects 1 Hz updates, so has a 2 second timeout
# 
# Follows https://pyserial.readthedocs.org/en/latest/pyserial_api.html
#
# Last Revised by Sushant Sundaresh on 21 January 2016

# Useful Resources
# Configure & view /dev/ttyACM0 COM stream at 9600 baud, odd parity, 8 bits
# 	http://playground.arduino.cc/Interfacing/Python
# 	use make listen
# Check USB devices attached (to find COM port)
# 	http://playground.arduino.cc/Interfacing/LinuxTTY
# 	cd /dev
# 	ls tty*
# 	for me, so far, always ACM0

import serial
import sys

if __name__ == "__main__":
	count = 0

	print "Listening for Uno:\n"
	with serial.Serial(	port='/dev/ttyACM0', baudrate=9600, \
						bytesize=serial.EIGHTBITS, \
						parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE, 
						timeout=3) as uno:
		while (count < 100):
			count += 1
			x = uno.read(size=1)
			if (len(x) == 0):
				print "\nUno appears silent.\n"
				break			
			print "%d: %s\n" % (count, x)
		
		if (count == 100):
			print "\nTest successful.\n"	

	sys.exit(0);



