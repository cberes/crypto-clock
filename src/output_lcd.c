#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "rpiGpio.h"
#include "output.h"

#define LCD_RS 4
#define LCD_E 17
#define LCD_DATA4 18
#define LCD_DATA5 22
#define LCD_DATA6 23
#define LCD_DATA7 24

#define LCD_WIDTH 16 		// width in characters
#define E_PULSE_MICROS 100
#define RS_SETUP_NANOS 40

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

static void generate_clock(void) {
    sleep_micros(1);
    gpioSetPin(LCD_E, high);
    sleep_micros(1);
    gpioSetPin(LCD_E, low);
    sleep_micros(0, E_PULSE_MICROS);
}

static void lcd_send_nibble(int bits, eState mode) {
    gpioSetPin(LCD_RS, mode);
    gpioSetPin(LCD_DATA4, (bits & 0x01) != 0 ? high : low);
    gpioSetPin(LCD_DATA5, (bits & 0x02) != 0 ? high : low);
    gpioSetPin(LCD_DATA6, (bits & 0x04) != 0 ? high : low);
    gpioSetPin(LCD_DATA7, (bits & 0x08) != 0 ? high : low);

    generate_clock();
}

static void lcd_send_byte(int bits, eState mode) {
    lcd_send_nibble(bits >> 4, mode);
    lcd_send_nibble(bits, mode);
}

static void select_line(int line) {
    lcd_send_byte(line == 0 ? 0x80 : 0xC0, commandMode());
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
    }
}

void output_line(int line, char *message, int align) {
    select_line(line);
    lcd_send_string(message, ' ', align);
}

void lcd_init(void) {
    eState mode = commandMode();
    lcd_send_byte(0x03, mode);
    sleep_millis(5);
    lcd_send_byte(0x03, mode);
    sleep_millis(5);
    lcd_send_byte(0x03, mode);
    sleep_micros(100);
    lcd_send_byte(0x02, mode);
    sleep_micros(50);
    lcd_send_byte(0x28, mode); // use 4-bit mode, 2 lines and 5x8 mode
    sleep_micros(50);
    lcd_send_byte(0x08, mode); // display off
    sleep_micros(50);
    lcd_send_byte(0x01, mode); // display clear
    sleep_micros(50);
    lcd_send_byte(0x06, mode); // entry mode set
    sleep_micros(50);
    // lcd_send_byte(0x0C, mode); // display on TODO need this?
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
