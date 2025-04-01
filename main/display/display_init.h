#ifndef _DISPLAY_INIT_H_
#define _DISPLAY_INIT_H_

#include "esp_lcd_types.h"

// Display resolution
#define LCD_WIDTH   320
#define LCD_HEIGHT  170

#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

// Data bus GPIOs (D0-D7)
#define LCD_DATA0      39
#define LCD_DATA1      40
#define LCD_DATA2      41
#define LCD_DATA3      42
#define LCD_DATA4      45
#define LCD_DATA5      46
#define LCD_DATA6      47
#define LCD_DATA7      48

// Control signals
#define LCD_WR              8
#define LCD_RD              9
#define LCD_DC              7
#define LCD_CS              6
#define LCD_RST             5
#define GPIO_LCD_BACKLIGHT  38

// Pixel clock
#define LCD_PIXEL_CLOCK_HZ (16 * 1000 * 1000)

esp_lcd_panel_handle_t display_init(void);

#endif // _DISPLAY_INIT_H_