import serial # Package needed to manage data going through serial port
import time

# Initialization phase
vcd_file = open('acquired_data.vcd', 'w')

portpath = '/dev/ttyACM0'
baudrate = 9600
serialport = serial.Serial(portpath, 9600)
serialport.open()

if(serialport.isOpen()):
	print 'Serial port opened at', path, 'set at a baudrate of', baudrate, 'Bd'

# Write .vcd header
date = time.strftime('%a %B %d %Y')
vcd_file.write('$date\n\t' + date + '\n$end\n'
	+ '$timescale 1us $end\n$scope module logic $end\n')

nlines = 2 # The number of line cannot exceed 8
syms = ['#', '$', '%', '&', '(', '?', '§', '@'] # Symbols used to refer to the input wires
names = ['databit', 'clock', 'l2', 'l3', 'l4', 'l5', 'l6', 'l7'] # Context-linked names associated to each input wire
for i in range(nlines):
	vcd_file.write('$var wire 1 ' + syms[i] + ' ' + names[i] + ' $end\n')

vcd_file.write('$upscope $end\n$enddefinitions $end\n')
# Now the .vcd file is ready to receive the variable dumps


# # Reading loop

# Default starting values are zeros
old_data = 0
vcd_file.write('#0\n')
for i in range(nlines):
	vcd_file.write('0'+ syms[i] + '\n')

# Starting time
start = time.clock()

# Scrutinize the data going through the serial port and write them down on the .vcd file
while(serialport.isOpen()):
	curr_data = serialport.read(1) # Read exactly one byte from the serial port buffer
	timediff = time.clock() - start

	if (curr_data != old_data): # Check if the global data has changed
		vcd_file.write('#' + str(int(1e6 * timediff)) + '\n')

		for i in range(nlines):
			curr_ibit = (curr_data & (1 << i) ) >> i # Extract the i-th bit from the current and old data bytes
			old_ibit = (old_data & (1 << i) ) >> i
			if curr_ibit != old_ibit:
				vcd_file.write(str(curr_ibit) + syms[i] + '\n')

		old_data = curr_data # Ultimately, update memory of previously received data


# Termination phase
timestamp = str(int(1e6 * time.clock() - start) )
vcd_file.write('#' + timestamp)
vcd_file.close()