#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <jansson.h>
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

int parse_response(char *text, char *price, int max_length) {
    json_t *root, *data, *amount;
    json_error_t error;
    const char* value;

    root = json_loads(text, 0, &error);

    if (!json_is_object(root)) {
        fprintf(stderr, "json error: response is not an object\n");
        json_decref(root);
        return 10;
    }

    data = json_object_get(root, "data");

    if (!json_is_object(data)) {
        fprintf(stderr, "json error: response data is not an object\n");
        json_decref(root);
        return 11;
    }

    amount = json_object_get(data, "amount");

    if (!json_is_string(amount)) {
        fprintf(stderr, "json error: response amount is not a string\n");
        json_decref(root);
        return 12;
    }

    value = json_string_value(amount);

    strncpy(price, value, max_length);
    price[max_length - 1] = '\0';

    json_decref(root);
    return 0;
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

int price_for(char *id, char *price, int max_length) {
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
        return_code = parse_response(chunk.memory, price, max_length);
    }

    curl_easy_cleanup(curl);
 
    free(chunk.memory);

    return return_code;
}
