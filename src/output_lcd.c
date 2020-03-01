#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "rpiGpio.h"
#include "output.h"

#define LCD_RS 4        // pin  7
#define LCD_E 17        // pin 11
#define LCD_DATA4 18    // pin 12
#define LCD_DATA5 22    // pin 15
#define LCD_DATA6 23    // pin 16
#define LCD_DATA7 24    // pin 18

#define LCD_WIDTH 16            // width in characters
#define RS_SETUP_NANOS 50       // minimum time between setup of RS and rising clock edge
#define DATA_SETUP_NANOS 100    // minimum time between setup of data and falling clock edge
#define DATA_HOLD_NANOS 20      // minimum time to hold date after falling clock edge
#define CLOCK_LEVEL_NANOS 250   // minimum total time for high or low clock pulse
#define EXECUTION_MICROS 50     // most commands require 38 us to execute; round up a litte

static eState commandMode() {
    return low;
}

static eState characterMode() {
    return high;
}

static void precise_sleep(time_t seconds, long nanos) {
    struct timespec req, rem;
    req.tv_sec = seconds;
    req.tv_nsec = nanos;

    if (nanosleep(&req, &rem) == -1) {
        precise_sleep(rem.tv_sec, rem.tv_nsec);
    }
}

void sleep_nanos(long nanos) {
    precise_sleep(0, nanos);
}

void sleep_micros(long micros) {
    precise_sleep(0, micros * 1000);
}

void sleep_millis(long millis) {
    precise_sleep(0, millis * 1000 * 1000);
}

static void lcd_send_nibble(int bits, eState mode) {
    gpioSetPin(LCD_RS, mode);

    sleep_nanos(RS_SETUP_NANOS);
    gpioSetPin(LCD_E, high);

    gpioSetPin(LCD_DATA4, (bits & 0x01) != 0 ? high : low);
    gpioSetPin(LCD_DATA5, (bits & 0x02) != 0 ? high : low);
    gpioSetPin(LCD_DATA6, (bits & 0x04) != 0 ? high : low);
    gpioSetPin(LCD_DATA7, (bits & 0x08) != 0 ? high : low);

    sleep_nanos(DATA_SETUP_NANOS);
    sleep_nanos(CLOCK_LEVEL_NANOS - DATA_SETUP_NANOS);
    gpioSetPin(LCD_E, low);

    sleep_nanos(DATA_HOLD_NANOS);
    gpioSetPin(LCD_DATA4, low);
    gpioSetPin(LCD_DATA5, low);
    gpioSetPin(LCD_DATA6, low);
    gpioSetPin(LCD_DATA7, low);

    sleep_nanos(CLOCK_LEVEL_NANOS - DATA_HOLD_NANOS);
}

static void lcd_send_byte(int bits, eState mode) {
    lcd_send_nibble(bits >> 4, mode);
    lcd_send_nibble(bits, mode);
}

static void select_line(int line) {
    lcd_send_byte(line == 0 ? 0x80 : 0xC0, commandMode());
    sleep_micros(EXECUTION_MICROS);
}

static char char_at_index(char *message, int i, int length, char padWith, int align) {
    switch (align) {
        case OUTPUT_CENTER_ALIGN:
            // TODO
        case OUTPUT_RIGHT_ALIGN:
            if (length >= LCD_WIDTH) {
                return message[i];
            } else if (length + i >= LCD_WIDTH) {
                return message[i - (LCD_WIDTH - length)];
            } else {
                return padWith;
            }
        case OUTPUT_LEFT_ALIGN:
        default:
            return i < length ? message[i] : padWith;
    }
}

void lcd_send_string(char *message, char padWith, int align) {
    const int length = strlen(message);
    int i;
    for (i = 0; i < LCD_WIDTH; ++i) {
        lcd_send_byte(char_at_index(message, i, length, padWith, align), characterMode());
        sleep_micros(EXECUTION_MICROS);
    }
}

void output_line(int line, char *message, int align) {
    select_line(line);
    lcd_send_string(message, ' ', align);
}

void lcd_init(void) {
    eState mode = commandMode();
    // 4-bit mode initialization
    lcd_send_byte(0x03, mode);
    sleep_millis(5);
    lcd_send_byte(0x03, mode);
    sleep_millis(5);
    lcd_send_byte(0x03, mode);
    sleep_micros(100);
    lcd_send_byte(0x02, mode);
    sleep_micros(100);
    // device configuration
    lcd_send_byte(0x28, mode); // use 4-bit mode, 2 lines and 5x8 mode
    sleep_micros(EXECUTION_MICROS);
    lcd_send_byte(0x0C, mode); // display on, cursor off
    sleep_micros(EXECUTION_MICROS);
    lcd_send_byte(0x01, mode); // display clear
    sleep_millis(2);
    lcd_send_byte(0x06, mode); // entry mode set: move cursor to right, no shifting
    sleep_micros(EXECUTION_MICROS);
}

void gpio_init(void) {
    gpioSetFunction(LCD_RS, output);
    gpioSetFunction(LCD_E, output);
    gpioSetFunction(LCD_DATA4, output);
    gpioSetFunction(LCD_DATA5, output);
    gpioSetFunction(LCD_DATA6, output);
    gpioSetFunction(LCD_DATA7, output);

    gpioSetPin(LCD_RS, low);
    gpioSetPin(LCD_E, low);
}

int output_init(void) {
    if (gpioSetup() != OK) {
        fprintf(stderr, "gpio setup failed\n");
        return 1;
    }

    gpio_init();

    sleep_micros(1);

    lcd_init();

    return 0;
}

void output_close(void) {
    gpioCleanup();
}
