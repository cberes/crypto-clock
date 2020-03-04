#ifndef PRICE_ELEMENT_H
#define PRICE_ELEMENT_H

#define NAME_MAX_LENGTH 32
#define PRICE_MAX_LENGTH 16

typedef struct price_element {
    char name[NAME_MAX_LENGTH];
    char price[PRICE_MAX_LENGTH];
} price_element;

#endif
