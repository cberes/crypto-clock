CC=gcc
CFLAGS=-I.

all: output_console.o price_data.o
	$(CC) $(CFLAGS) -o main src/main.c output_console.o price_data.o -lcurl -ljansson

output_lcd.o:
	$(CC) $(CFLAGS) -I../RaspberryPi-GPIO/include -c src/output_lcd.c ../RaspberryPi-GPIO/library/*.a

output_console.o:
	$(CC) $(CFLAGS) -c src/output_console.c

price_data.o:
	$(CC) $(CFLAGS) -c src/price_data.c

clean:
	rm *.o
	rm main
