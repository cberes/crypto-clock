#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int output_init(void) {
    // do nothing
    return 0;
}

void output_close(void) {
    // do nothing
}

void output_line(int line, char* text, int align) {
    printf("%d -> %s\n", line, text);
}
