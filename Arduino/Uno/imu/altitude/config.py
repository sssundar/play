# @file config.py
# @details Creates config.h header file 
# @author Sushant Sundaresh
# @date 2016-11-30

import sys

if __name__ == "__main__":
	if (len(sys.argv) != 2):
		print "Error. Usage: python config.py [gcc|avr]"
		sys.exit(1)

	CONFIG = ""
	CONFIG += "//Automatically Generated File. Changes will be overwritten.\n"
	CONFIG += "#ifndef _CONFIG_H_\n"
	CONFIG += "#define _CONFIG_H_\n"
	if (sys.argv[1] == "gcc"):
		CONFIG += "#define AVR 0\n"
	elif (sys.argv[1] == "avr"):
		CONFIG += "#define AVR 1\n"
	CONFIG += "#endif\n"
	CONFIG += "// _CONFIG_H_\n"

	print CONFIG
	sys.exit(0)
