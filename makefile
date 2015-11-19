CC=avr-gcc

readinput: readinput.c
	$(CC) -o readinput.out -Os readinput.c -mmcu=atmega328p
	avr-objcopy -O binary readinput.out readinput.bin
	avrdude -p m328p -P /dev/ttyACM0 -c arduino -U flash:w:readinput.bin
