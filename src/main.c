#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <jansson.h>
#include "rpiGpio.h"

#define LCD_RS 4
#define LCD_E 17
#define LCD_DATA4 18
#define LCD_DATA5 22
#define LCD_DATA6 23
#define LCD_DATA7 24

#define LCD_WIDTH 16 		// width in characters

#define E_PULSE_MICROS 100
#define RS_SETUP_NANOS 40
#define DATA_HOLD_NANOS 10

eState commandMode() {
    return low;
}

eState characterMode() {
    return high;
}

void precise_sleep(time_t seconds, long nanos) {
    struct timespec req, rem;
    req.tv_sec = seconds;
    req.tv_nsec = nanos;

    if (nanosleep(&req, &rem) == -1) {
        precise_sleep(rem.tv_sec, rem.tv_nsec);
    }
}

void generate_clock(void) {
    // according to LCD documentation, 100-us clock pulse is overkill
    // but lower values seem not to work
    gpioSetPin(LCD_E, high);
    usleep(E_PULSE_MICROS);
    gpioSetPin(LCD_E, low);
    usleep(E_PULSE_MICROS);
}

void lcd_send_nibble(int bits, eState mode) {
    // set mode
    gpioSetPin(LCD_RS, mode);
    precise_sleep(0, RS_SETUP_NANOS); // TODO is this necessary?

    // set data
    gpioSetPin(LCD_DATA4, (bits & 0x01) != 0 ? high : low);
    gpioSetPin(LCD_DATA5, (bits & 0x02) != 0 ? high : low);
    gpioSetPin(LCD_DATA6, (bits & 0x04) != 0 ? high : low);
    gpioSetPin(LCD_DATA7, (bits & 0x08) != 0 ? high : low);

    generate_clock();
}

void lcd_send_byte(int bits, eState mode) {
    lcd_send_nibble(bits >> 4, mode);
    lcd_send_nibble(bits, mode);
}

void select_line(int line) {
    lcd_send_byte(line == 0 ? 0x80 : 0xC0, commandMode());
}

void lcd_send_string(char* message, int width, char padWith) {
    const int length = strlen(message);
    int i;
    for (i = 0; i < width; ++i) {
        lcd_send_byte(i < length ? message[i] : padWith, characterMode());
    }
}

void lcd_message(int line, char* message) {
    select_line(line);
    lcd_send_string(message, LCD_WIDTH, ' ');
}

void lcd_init(void) {
    eState mode = commandMode();
    lcd_send_nibble(3, mode); // 8-bit mode
    usleep(5000);
    lcd_send_nibble(3, mode); // 8-bit mode
    usleep(150);
    lcd_send_nibble(3, mode); // 8-bit mode
    lcd_send_nibble(2, mode); // 4-bit mode
    lcd_send_byte(0x28, mode); // use 4-bit mode, 2 lines and 5x8 mode
    lcd_send_byte(0x08, mode); // display off
    lcd_send_byte(0x01, mode); // display clear
    lcd_send_byte(0x06, mode); // entry mode set
    lcd_send_byte(0x0C, mode); // display on
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

int main(void) {
    if (gpioSetup() != OK) {
        dbgPrint(DBG_INFO, "gpioSetup failed. Exiting");
        return 1;
    }

    gpio_init();

    lcd_init();

    lcd_message(0, "This is line 1");
    lcd_message(1, "Wow, now line 2.");

    gpioCleanup();
    return 0;
}
