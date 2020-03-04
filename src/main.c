#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "price_element.h"
#include "price_data.h"
#include "output.h"

#define PRICE_DELAY 50
#define OUTPUT_DELAY 15
#define PRICE_COUNT 1

void sigint_handler(int sig_num)  {
    printf("Exiting...\n"); 

    price_close();

    output_close();

    exit(0);
}

int main(void) {
    int return_code = 0;
    char *price_ids[] = {"Bitcoin"};
    struct price_element prices[PRICE_COUNT];
    int i;

    return_code = output_init();
    if (return_code) {
        return return_code;
    }

    price_init();

    signal(SIGINT, sigint_handler); 

    while (1) {
        return_code = prices_for(price_ids, prices, PRICE_COUNT);

        if (!return_code) {
            for (i = 0; i < PRICE_COUNT; ++i) {
                output_line(0, prices[i].name, OUTPUT_LEFT_ALIGN);
                output_line(1, prices[i].price, OUTPUT_RIGHT_ALIGN);
                sleep(OUTPUT_DELAY);
            }
        }

        sleep(PRICE_DELAY - OUTPUT_DELAY * PRICE_COUNT); // TODO what if this is negative?
    }
}
