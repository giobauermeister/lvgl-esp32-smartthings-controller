#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdbool.h>

#define BUTTON_GPIO 14

void button_init(void);
void button_set_callback(void (*cb)(void));
void button_set_longpress_callback(void (*cb)(void));
bool button_is_pressed(void);

#endif // _BUTTON_H_