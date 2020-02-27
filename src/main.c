#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
#define LCD_LINE_1 0x80 	// address of first display line
#define LCD_LINE_2 0xC0 	// address of second display line
#define E_PULSE 500         // microseconds
#define E_DELAY 500         // microseconds

eState commandMode() {
    return low;
}

eState characterMode() {
    return high;
}

void lcd_send_byte(int bits, int mode) {
    gpioSetPin(LCD_RS, mode;
    gpioSetPin(LCD_DATA4, bits & 0x10 != 0 ? high : low);
    gpioSetPin(LCD_DATA5, bits & 0x20 != 0 ? high : low);
    gpioSetPin(LCD_DATA6, bits & 0x40 != 0 ? high : low);
    gpioSetPin(LCD_DATA7, bits & 0x80 != 0 ? high : low);

    usleep(E_DELAY);
    gpioSetPin(LCD_E, high);
    usleep(E_PULSE);
    gpioSetPin(LCD_E, low);
    usleep(E_DELAY);

    gpioSetPin(LCD_DATA4, bits & 0x01 != 0 ? high : low);
    gpioSetPin(LCD_DATA5, bits & 0x02 != 0 ? high : low);
    gpioSetPin(LCD_DATA6, bits & 0x04 != 0 ? high : low);
    gpioSetPin(LCD_DATA7, bits & 0x08 != 0 ? high : low);

    usleep(E_DELAY);
    gpioSetPin(LCD_E, high);
    usleep(E_PULSE);
    gpioSetPin(LCD_E, low);
    usleep(E_DELAY);
}

void select_line(int line) {
	lcd_send_byte(line == 0 ? LCD_LINE_1 : LCD_LINE_2, commandMode());
}

void lcd_message(char[] message) {
    const int length = strlen(message);
    for (int i = 0; i < LCD_WIDTH; ++i) {
        lcd_send_byte(i < length ? message[i] : ' ', characterMode());
    }
}

void lcd_init(void) {
	lcd_send_byte(0x33, commandMode());
	lcd_send_byte(0x32, commandMode());
	lcd_send_byte(0x28, commandMode());
	lcd_send_byte(0x0C, commandMode()); 
	lcd_send_byte(0x06, commandMode());
	lcd_send_byte(0x01, commandMode());
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

	lcd_init()

	select_line(0);
	lcd_message("Es scheint zu");
	select_line(1);
	lcd_message("funktionieren :)");

    gpioCleanup();
    return 0;
}
