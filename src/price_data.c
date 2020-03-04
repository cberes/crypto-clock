#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include "price_element.h"
#include "coinbase_parser.h"
#include "price_data.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};

void price_init(void) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void price_close(void) {
    curl_global_cleanup();
}

size_t write_data(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

int price_for(char *id, price_element *price) {
    int return_code = 0;
    struct MemoryStruct chunk = {
        .memory = 0,
        .size = 0
    };
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (!curl) {
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.coinbase.com/v2/prices/spot?currency=USD");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl); 

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return_code = 2;
    } else {
        return_code = parse_response(chunk.memory, price);
        price->name = id;
    }

    curl_easy_cleanup(curl);
 
    free(chunk.memory);

    return return_code;
}

int prices_for(char **ids, price_element *prices, int length) {
    int return_code = 0;
    int last_return_code;
    int i;
    for (i = 0; i < length; ++i) {
        last_return_code = price_for(ids[i], &prices[i]);

        if (last_return_code) {
            return_code = last_return_code;
        }
    }
    return return_code;
}
