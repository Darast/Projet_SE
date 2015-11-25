import serial # Package needed to read data going through serial port

import time   # Module needed to timestamp data
import sys

# Initialization phase
vcd_file = open('acquired_data.vcd', 'w')

portpath = '/dev/ttyACM0'
baudrate = 1e6
serialport = serial.Serial(portpath, baudrate)

if not serialport.isOpen():
	serialport.open()

if(serialport.isOpen()):
	print 'Serial port opened at', portpath, 'set at a baudrate of', baudrate, 'Bd'
else:
	sys.exit(1)

# Write .vcd header
date = time.strftime('%a %B %d %Y')
vcd_file.write('$date\n\t' + date + '\n$end\n'
	+ '$timescale 1us $end\n$scope module logic $end\n')

nlines = 2 # The number of line cannot exceed 8
syms = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'] # Symbols used to refer to the input wires
names = ['databit', 'clock', 'l2', 'l3', 'l4', 'l5', 'l6', 'l7'] # Context-linked names associated to each input wire
for i in range(nlines):
	vcd_file.write('$var wire 1 ' + syms[i] + ' ' + names[i] + ' $end\n')

vcd_file.write('$upscope $end\n$enddefinitions $end\n')
# Now the .vcd file is ready to receive the variable dumps


# # Reading loop

# Default values are all zeros
old_data = 0
vcd_file.write('$dumpvars\n')
for i in range(nlines):
	vcd_file.write('0'+ syms[i] + '\n')

# Scrutinize the data going through the serial port and write them down on the .vcd file
first_byte = True
start = 0
count = 0

while(serialport.isOpen()):
	
	try:

		curr_data = serialport.read(1) # Read exactly one byte from the serial port buffer			
		curr_data = ord(curr_data)

		if (first_byte):
			start = time.clock()
			timediff = 0
			first_byte = False
		else:
			 timediff = time.clock() - start

		if (curr_data != old_data): # Check if the global data has changed
			vcd_file.write('#' + str(int(1e6 * timediff)) + '\n')

			for i in range(nlines):
				curr_ibit = (curr_data & (1 << i) ) >> i # Extract the i-th bit from the current and old data bytes
				old_ibit = (old_data & (1 << i) ) >> i
				if curr_ibit != old_ibit:
					vcd_file.write(str(curr_ibit) + syms[i] + '\n')

			old_data = curr_data # Ultimately, update memory of previously received data
			count += 1

	except KeyboardInterrupt:
		vcd_file.write('#' + str(int(1e6 * (time.clock() - start))))
		vcd_file.close()
		serialport.close()
		quit()

		