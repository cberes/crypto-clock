#ifndef PRICE_ELEMENT_H
#define PRICE_ELEMENT_H

#define PRICE_MAX_LENGTH 32

typedef struct price_element {
    char *name;
    char price[PRICE_MAX_LENGTH];
} price_element;

#endif
