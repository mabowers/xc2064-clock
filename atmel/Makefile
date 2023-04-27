CC = avr-gcc
CFLAGS = -mmcu=attiny412
CFLAGS += -Os
CFLAGS += -Wall

xc2064-loader:
	$(CC) $(CFLAGS) main.cpp -o xc2064-loader.elf

clean:
	rm -rf *.elf
