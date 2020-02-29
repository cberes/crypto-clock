#ifndef PRICE_DATA_H
#define PRICE_DATA_H

void price_init(void);

void price_close(void);

int price_for(char *id, char *price, int max_length);

#endif
