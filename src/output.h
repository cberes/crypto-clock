#ifndef OUTPUT_H
#define OUTPUT_H

#define OUTPUT_LEFT_ALIGN   0
#define OUTPUT_CENTER_ALIGN 1
#define OUTPUT_RIGHT_ALIGN  2

int output_init(void);

void output_close(void);

void output_line(int line, char *text, int align);

#endif