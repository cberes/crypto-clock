CC=gcc
CFLAGS=-Wall -Werror -I.

all: output_lcd.o price_data.o coinbase_parser.o
	$(CC) $(CFLAGS) -o main src/main.c output_lcd.o price_data.o coinbase_parser.o ../RaspberryPi-GPIO/library/*.a -lcurl -ljansson

desktop: output_console.o price_data.o coinbase_parser.o
	$(CC) $(CFLAGS) -o main src/main.c output_console.o price_data.o coinbase_parser.o -lcurl -ljansson

output_lcd.o:
	$(CC) $(CFLAGS) -I../RaspberryPi-GPIO/include -c src/output_lcd.c

output_console.o:
	$(CC) $(CFLAGS) -c src/output_console.c

price_data.o:
	$(CC) $(CFLAGS) -c src/price_data.c

coinbase_parser.o:
	$(CC) $(CFLAGS) -c src/coinbase_parser.c

clean:
	rm -f *.o
	rm -f main
