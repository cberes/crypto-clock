# crypto-clock

Gets crypto prices and outputs them to a 16x2 LCD display using a Raspberry Pi.

## Dependencies

- [libcurl](https://curl.haxx.se/libcurl/)
- [Jansson](https://github.com/akheron/jansson)
- [RaspberryPi-GPIO](https://github.com/alanbarr/RaspberryPi-GPIO)

## Configuration

### API key

crypto-clock uses the [CoinMarketCap](https://coinmarketcap.com) API. Before running the program,
you will need to provide your API key via an environment variable. For example, run

    export CRYPTO_API_KEY=7892b338-813b-458b-a89a-9ef76924b3c2

### LCD

Connect your LCD pins to your Raspberry Pi. The :CD's RW pin must be tied to ground.

| LCD    | GPIO | Model 1 B pin |
| ------ | ---: | ------------: |
| RS     |    4 |             7 |
| E      |   17 |            11 |
| Data 4 |   18 |            12 |
| Data 5 |   22 |            15 |
| Data 6 |   23 |            16 |
| Data 7 |   24 |            18 |

## Build & run

### Raspberry Pi

In the root project directory, run

    make

and then run `main` with the cryptocurrency slugs of your choice

    sudo ./main bitcoin ethereum litecoin

### Desktop

In the root project directory, run

    make desktop

and then run `main` with the cryptocurrency slugs of your choice

    ./main bitcoin ethereum litecoin
