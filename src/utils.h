#ifndef UTILS
#define UTILS

void get_terminal_size(int *width, int *height);
void sleep_ms(int ms);
void clear_terminal(void);

void random_int_init(void);
int random_int(int min, int max);

#endif