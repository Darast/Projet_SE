from Verilog_VCD import parse_vcd # Parse .vcd file to Python data structure

# # PS/2 frame consists in
# 1 start bit set at 0
# 8 databits
# 1 parity bit
# 1 stop bit set at 1

def find_val_at_t(values, t): # Get the value of line at time t
	ti = -1
	i = -1
	while ti < t:
		i += 1
		ti = values[i][0]

	return int(values[i - 1][1])


def extract_from_vcd(filename):
	rawdata = parse_vcd(filename) # Get raw data from file

	# Find the lines corresponding to clock and databit
	for key in rawdata.keys():
		if (rawdata[key]['nets'][0]['size'] == '1'):
			if (rawdata[key]['nets'][0]['name'] == 'databit'): # databit will be shortened as db
				db_line = key
			elif (rawdata[key]['nets'][0]['name'] == 'clock'): # clock will be shortened as ck
				ck_line = key

	db_values = rawdata[db_line]['tv']
	ck_values = rawdata[ck_line]['tv']

	# Clock tick counter, used to find a bit's position in the PS/2 frame
	ck_count = 0 # 0 is start bit, 10 is stop bit
	databyte = 0 # Current databyte
	csum = 0 # Check sum

	extracted_bytes = []

	for (t, ck) in ck_values:
		if ck == '1': # If the value considered is a clock tick

			db = find_val_at_t(db_values, t) # Find the databit value at that time
			bitpos = ck_count % 11

			if (bitpos == 0) and (db == 0): # If the considered bit is the start bit
				databyte = 0 # Reset the "output" databyte,
				csum = 0 # reset the check sum
				ck_count += 1 # and read the folowing bits
				print 'Start bit'
			elif (bitpos < 9): # If the considered bit is a databit
				databyte += (db << (bitpos - 1))
				csum += db
				ck_count += 1
			elif (bitpos == 9) and (csum % 2 + 1 == db): # If the considered bit is the parity bit and if its value is correct
				extracted_bytes.append(databyte)
				ck_count += 1
				print 'Parity bit'
			elif (bitpos == 10) and (db == 1): # If the considered bit is the stop bit
				ck_count += 1 # Ignore it and read the folowing bits
				print 'Stop bit'

			# # # This code WILL skip some bits if they don't match PS/2 frame !!

	return extracted_bytes

dat = extract_from_vcd('acquired_data.vcd')
print dat