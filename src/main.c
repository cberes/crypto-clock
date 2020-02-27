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

#define E_PULSE_NANOS 230
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

void lcd_send_nibble(int bits, eState mode) {
    // set mode
    gpioSetPin(LCD_RS, mode);
    precise_sleep(0, RS_SETUP_NANOS);

    // high clock
    gpioSetPin(LCD_E, high);

    // set data
    gpioSetPin(LCD_DATA4, bits & 0x01 != 0 ? high : low);
    gpioSetPin(LCD_DATA5, bits & 0x02 != 0 ? high : low);
    gpioSetPin(LCD_DATA6, bits & 0x04 != 0 ? high : low);
    gpioSetPin(LCD_DATA7, bits & 0x08 != 0 ? high : low);

    precise_sleep(0, E_PULSE_NANOS);

    // low clock + hold data briefly
    gpioSetPin(LCD_E, low);
    precise_sleep(0, DATA_HOLD_NANOS);

    // clear data
    gpioSetPin(LCD_DATA4, bits & 0x01 != 0 ? high : low);
    gpioSetPin(LCD_DATA5, bits & 0x02 != 0 ? high : low);
    gpioSetPin(LCD_DATA6, bits & 0x04 != 0 ? high : low);
    gpioSetPin(LCD_DATA7, bits & 0x08 != 0 ? high : low);

    precise_sleep(0, E_PULSE_NANOS - DATA_HOLD_NANOS);
}

void lcd_send_byte(int bits, eState mode) {
    lcd_send_nibble(bits >> 4, mode);
    lcd_send_nibble(bits, mode);
}

void select_line(int line) {
	lcd_send_byte(line == 0 ? 0x80 : 0xC0, commandMode());
}

void lcd_message(char[] message) {
    const int length = strlen(message);
    int i;
    for (i = 0; i < LCD_WIDTH; ++i) {
        lcd_send_byte(i < length ? message[i] : ' ', characterMode());
    }
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
}

int main(void) {
    if (gpioSetup() != OK) {
        dbgPrint(DBG_INFO, "gpioSetup failed. Exiting");
        return 1;
    }

    gpioSetFunction(LCD_RS, output);
    gpioSetFunction(LCD_E, output);
    gpioSetFunction(LCD_DATA4, output);
    gpioSetFunction(LCD_DATA5, output);
    gpioSetFunction(LCD_DATA6, output);
    gpioSetFunction(LCD_DATA7, output);

	lcd_init();

	select_line(0);
	lcd_message("This is line 1");
	select_line(1);
	lcd_message("Wow, now line 2.");

    gpioCleanup();
    return 0;
}
