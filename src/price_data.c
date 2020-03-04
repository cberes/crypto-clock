#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include "price_element.h"
#include "coinmarketcap_parser.h"
#include "price_data.h"

#define REQUEST_MAX_LENGTH 500

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

static char *build_request(char *result, char **ids, int count) {
    char *prefix = "slug=";
    int offset = 0;
    int i;

    for (i = 0; i < count; ++i) {
        if (offset == 0) {
            strcpy(result, prefix);
            offset += strlen(prefix);
        } else {
            result[offset++] = ',';
        }

        strcpy(result + offset, ids[i]);
        offset += strlen(ids[i]);
    }

    result[offset] = '\0';

    return result;
}

int prices_for(char **ids, char *api_key, price_element *prices, int length) {
    int return_code = 0;
    const char *url = "https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest";
    char api_key_header[56];
    char request_data[REQUEST_MAX_LENGTH];
    char url_with_data[REQUEST_MAX_LENGTH + strlen(url) + 1]; // 1 byte for '?'
    struct MemoryStruct chunk = {
        .memory = 0,
        .size = 0
    };
    struct curl_slist *headers = NULL;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (!curl) {
        return 1;
    }

    sprintf(api_key_header, "X-CMC_PRO_API_KEY: %s", api_key);

    build_request(request_data, ids, length);
    sprintf(url_with_data, "%s?%s", url, request_data);

    curl_easy_setopt(curl, CURLOPT_URL, url_with_data);
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, api_key_header);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl); 

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return_code = 2;
    } else {
        return_code = parse_response(chunk.memory, prices, length);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    free(chunk.memory);

    return return_code;
}
