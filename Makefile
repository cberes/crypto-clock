CC=gcc
CFLAGS=-I. -I../RaspberryPi-GPIO/include

main:
	$(CC) $(CFLAGS) -o main src/main.c ../RaspberryPi-GPIO/library/*.a

clean:
	rm main
