#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h> 
#include "price_data.h"
#include "output.h"

#define PRICE_DELAY 60
#define MAX_LENGTH 20

void sigint_handler(int sig_num)  {
    printf("Exiting...\n"); 

    price_close();

    output_close();

    exit(0);
}

int main(void) {
    int return_code = 0;
    char price[MAX_LENGTH];
    char *id = "Bitcoin";

    return_code = output_init();
    if (return_code) {
        return return_code;
    }

    price_init();

    signal(SIGINT, sigint_handler); 

    while (1) {
        return_code = price_for(id, price, MAX_LENGTH);

        if (!return_code) {
            output_line(0, id, OUTPUT_LEFT_ALIGN);
            output_line(1, price, OUTPUT_RIGHT_ALIGN);
        }

        sleep(PRICE_DELAY);
    }
}
