# crypto-clock

Gets crypto prices and outputs them to a 16x2 LCD display using a Raspberry Pi.

## Dependencies

- [libcurl](https://curl.haxx.se/libcurl/)
- [Jansson](https://github.com/akheron/jansson)
- [RaspberryPi-GPIO](https://github.com/alanbarr/RaspberryPi-GPIO)

## Build & run

### Raspberry Pi

In the root project directory, run

    make

and then

    sudo ./main

### Desktop

In the root project directory, run

    make desktop

and then

    ./main
