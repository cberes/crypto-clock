#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <jansson.h>
#include "price_element.h"
#include "coinbase_parser.h"

int parse_response(char *text, price_element *price_elem) {
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

    strncpy(price_elem->price, value, PRICE_MAX_LENGTH);
    price_elem->price[PRICE_MAX_LENGTH - 1] = '\0';

    json_decref(root);
    return 0;
}
