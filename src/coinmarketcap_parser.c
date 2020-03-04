#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <jansson.h>
#include "price_element.h"
#include "coinmarketcap_parser.h"

int parse_response(char *text, price_element *price_elems, int max_length) {
    json_t *root, *data, *datum, *name, *quote, *usd, *amount;
    json_error_t error;
    const char *id, *name_value;
    double price_value;
    int i = 0;

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

    json_object_foreach(data, id, datum) {
        if (i >= max_length) {
            continue;
        }

        name = json_object_get(datum, "name");

        if (!json_is_string(name)) {
            fprintf(stderr, "json error: response name is not a string\n");
            json_decref(root);
            return 12;
        }

        name_value = json_string_value(name);

        quote = json_object_get(datum, "quote");

        if (!json_is_object(quote)) {
            fprintf(stderr, "json error: %s quote is not an object\n", name_value);
            json_decref(root);
            return 13;
        }

        usd = json_object_get(quote, "USD");

        if (!json_is_object(usd)) {
            fprintf(stderr, "json error: %s USD quote is not an object\n", name_value);
            json_decref(root);
            return 14;
        }

        amount = json_object_get(usd, "price");

        if (!json_is_number(amount)) {
            fprintf(stderr, "json error: %s price is not a number\n", name_value);
            json_decref(root);
            return 15;
        }

        price_value = json_number_value(amount);

        strncpy(price_elems[i].name, name_value, NAME_MAX_LENGTH);
        price_elems[i].name[NAME_MAX_LENGTH - 1] = '\0';

        snprintf(price_elems[i].price, PRICE_MAX_LENGTH, "%f", price_value);

        ++i;
    }

    json_decref(root);
    return 0;
}
