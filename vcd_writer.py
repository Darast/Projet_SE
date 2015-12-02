import serial # Package needed to read data going through serial port

import sys
import time

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
ilines = [7, 6, 0, 0, 0, 0, 0, 0] # @DEBUG
for i in range(nlines):
	vcd_file.write('$var wire 1 ' + syms[i] + ' ' + names[i] + ' $end\n')

vcd_file.write('$upscope $end\n$enddefinitions $end\n')
# Now the .vcd file is ready to receive the variable dumps


# # Reading loop

# Default values are all zeros
old_data = 0
vcd_file.write('$dumpvars\n')
for l in range(nlines):
	vcd_file.write('0'+ syms[l] + '\n')

# Scrutinize the data going through the serial port and write them down on the .vcd file
while(serialport.isOpen()):
	
	try:

		buff_data = serialport.read(5) # Read exactly five bytes from the serial port buffer
		lines_data = ord(buff_data[4]) # Extract the byte corresponding to the lines values
		timestamp = ord(buff_data[3]) + (ord(buff_data[2]) << 8) + (ord(buff_data[1]) << 16) + (ord(buff_data[0]) << 24)
		# print timestamp # @DEBUG

		if (lines_data != old_data): # Check if the global data has changed
			vcd_file.write('#' + str(timestamp) + '\n')

			for l in range(nlines):
				i = ilines[l]
				curr_ibit = (lines_data & (1 << i) ) >> i # Extract the i-th bit from the current and old data bytes
				old_ibit = (old_data & (1 << i) ) >> i
				if curr_ibit != old_ibit: # Only print the values that have changed
					vcd_file.write(str(curr_ibit) + syms[i] + '\n')

			old_data = lines_data # Ultimately, update memory of previously received data

	except KeyboardInterrupt:
		vcd_file.write('#' + str( int(timestamp * 1.01) ) ) # Rough way to put a final timestamp a bit later than the last timestamp
		vcd_file.close()
		serialport.close()
		quit()

		