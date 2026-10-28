#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdbool.h>

#define BUTTON_GPIO 14

// The callbacks run in the button task, not in the LVGL task
void button_init(void);
void button_set_callback(void (*cb)(void));
void button_set_longpress_callback(void (*cb)(void));
bool button_is_pressed(void);

#endif // _BUTTON_H_