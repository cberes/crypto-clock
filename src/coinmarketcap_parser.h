#ifndef COINMARKETCAP_PARSER_H
#define COINMARKETCAP_PARSER_H

int parse_response(char *text, price_element *price_elems, int max_length);

#endif
