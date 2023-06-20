#ifndef UTILS
#define UTILS

void get_terminal_size(int *width, int *height);
void sleep_ms(int ms);
void clear_terminal(void);
void print_at(int x, int y, char *str);
void random_int_init(void);
int random_int(int min, int max);
void hide_cursor(void);
void show_cursor(void);

#endif