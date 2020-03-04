#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "price_element.h"
#include "price_data.h"
#include "output.h"

#define PRICE_ERROR_DELAY 120
#define OUTPUT_DELAY 15

void sigint_handler(int sig_num)  {
    printf("Exiting...\n"); 

    price_close();

    output_close();

    exit(0);
}

int main(void) {
    int return_code = 0;
    int price_count = 7;
    char *price_ids[] = {"bitcoin","ethereum","litecoin","bitcoin-cash","zcash","stellar","ripple"};
    char *api_key = getenv("CRYPTO_API_KEY");
    struct price_element prices[price_count];
    int i;

    return_code = output_init();
    if (return_code) {
        return return_code;
    }

    price_init();

    signal(SIGINT, sigint_handler); 

    while (1) {
        return_code = prices_for(price_ids, api_key, prices, price_count);

        if (!return_code) {
            for (i = 0; i < price_count; ++i) {
                output_line(0, prices[i].name, OUTPUT_LEFT_ALIGN);
                output_line(1, prices[i].price, OUTPUT_RIGHT_ALIGN);
                sleep(OUTPUT_DELAY);
            }
        } else {
            sleep(PRICE_ERROR_DELAY);
        }
    }
}
