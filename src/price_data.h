#ifndef PRICE_DATA_H
#define PRICE_DATA_H

void price_init(void);

void price_close(void);

int prices_for(char **ids, price_element *price_elems, int length);

#endif
